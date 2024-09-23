#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>


char* history[1000];
int history_size=0;
struct timespec duration[1000];
struct timespec start_time;
int pid_record[1000];
int pd;

//Separating the string and launching the command
int launch(char* whole_command) {
    char* command = strtok(whole_command, "|");
    char* commands[500];
    int num_commands = 0;

    while (command != NULL) {
        commands[num_commands++] = command;
        command = strtok(NULL, "|");
    }

    int fd[2];
    int input = 0;                                                              // Initial input is stdin (0)
    int pid;

    for (int j = 0; j < num_commands; j++) {

        char* arguments[500];
        char* token = strtok(commands[j], " \t\n");
        int bg=0;                                                               //flag for background command &
        int i = 0;
        while (token != NULL) {
            arguments[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        arguments[i] = NULL;

        //check for background
        if (i>0 && (strcmp(arguments[i-1],"&")==0)){                            // (arguments[i-1]=="&") not used cause in c memory address(pointers) are compared rather than the string
            arguments[--i]=NULL;                                                //strcmp returns 0 when same, <0 when first string lexographically smaller, >0 when second string lexographically smaller
            bg=1;
        }
        if (j != num_commands - 1) {                                            // Not the last command
            if (pipe(fd) == -1) {
                perror("Pipe failed");
                exit(1);
            }
        }

        pid = fork();
        if (pid < 0) {
            perror("Fork Failed");
            return 1;
        }
        if (pid == 0){
            if (input != 0) {                                                   // Previous command has been executed we want to read output of that command
                if (dup2(input, STDIN_FILENO) == -1) {
                    perror("dup2 failed for input redirection");
                    exit(1);
                }
                close(input);
            }

            if (j != num_commands - 1) {                                        // If not last command write here
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 failed for output");
                    exit(1);
                }
                close(fd[1]);                                                   // Write through referece in STDOUT
            }

            close(fd[0]);                                                       // Read end is not used

            if (execvp(arguments[0], arguments) == -1) {                        //Executing the current command
                perror("Command execution failed");
                exit(1);
            }
        }

        if (j==0){
            pd=pid;
        }

        if (!bg){
            waitpid(pid, NULL, 0);
        }

        if (input != 0) {
            close(input);                                                       // Closing previous pipe read
        }
        if (j != num_commands - 1) {
            close(fd[1]);
            input = fd[0];
        }
    }

    return 1;
}

char* read_user_input(){
    char* command=NULL;                             //Where the command would be stored.
    size_t commandSize=0;                           //Size allocated for command, getline can change it.
    getline(&command, &commandSize, stdin);         //reads from stdin.
    return command;
}


void show_history(){
    for (int i=0; i<history_size; i++){
        printf("%s\n",history[i]);
    }
}

void history_add(char* command, struct timespec time){
    int i=history_size;
    history_size++;
    struct timespec cur_time;
    clock_gettime(CLOCK_MONOTONIC, &cur_time);
    history[i]=strdup(command);
    duration[i].tv_sec=(cur_time.tv_sec-time.tv_sec);
    duration[i].tv_nsec=(cur_time.tv_nsec-time.tv_nsec);
    if (duration[i].tv_nsec<0){
        duration[i].tv_sec--;
        duration[i].tv_nsec+=1000000000;
    }
    pid_record[i]=pd;
}



void shell_loop(){
  int status;
  do{
    printf("shell: ");
    char* command = read_user_input();
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    if (strcmp(command, "history\n")==0){
        show_history();
        history_add(command, start_time);
        free(command);
        continue;
    }
    status = launch(command);
    history_add(command, start_time);
    free(command);
  } while (status);
}

int main(){
    shell_loop();
    return 0;
}
