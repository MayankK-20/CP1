#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void to_execute(char* whole_command) {
    char* command;
    char* commands[500];
    int num_commands = 0;
    
    // Split the entire command into separate commands using the pipe delimiter '|'
    command = strtok(whole_command, "|");
    while (command != NULL) {
        commands[num_commands++] = command;
        command = strtok(NULL, "|");
    }

    int fd[2];         // Pipe for passing data between commands
    int input_fd = 0;   // Initial input is stdin (0)

    int pid;
    for (int i = 0; i < num_commands; i++) {
        char* arguments[500];
        char* token = strtok(commands[i], " \t\n");
        int j = 0;
        while (token != NULL) {
            arguments[j++] = token;
            token = strtok(NULL, " \t\n");
        }
        arguments[j] = NULL;  // Null-terminate the argument list

        if (i != num_commands - 1) {
            // Create a pipe for the current command if it's not the last one
            if (pipe(fd) == -1) {
                perror("Pipe failed");
                exit(1);
            }
        }

        pid = fork();
        if (pid == 0) {  // Child process
            if (input_fd != 0) {
                // Redirect stdin to input_fd (previous pipe's read end)
                if (dup2(input_fd, STDIN_FILENO) == -1) {
                    perror("dup2 failed for input");
                    exit(1);
                }
                close(input_fd);  // Close the original input_fd after duplication
            }
            
            if (i != num_commands - 1) {
                // Redirect stdout to the write end of the current pipe
                if (dup2(fd[1], STDOUT_FILENO) == -1) {
                    perror("dup2 failed for output");
                    exit(1);
                }
                close(fd[1]);  // Close the original write end of the pipe
            }

            // Close the pipe read end in the child, it's unused here
            close(fd[0]);

            // Execute the command
            if (execvp(arguments[0], arguments) == -1) {
                perror("Command execution failed");
                exit(1);
            }
        }
        else if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }

        // Parent process: Close the write end of the current pipe and store read end for the next iteration
        if (input_fd != 0) {
            close(input_fd);  // Close previous pipe's read end
        }
        if (i != num_commands - 1) {
            close(fd[1]);  // Close the write end of the pipe in the parent
            input_fd = fd[0];  // Set input_fd to the read end of the current pipe for the next command
        }
    }

    // Parent waits for all child processes to finish
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}


int to_execute(char* whole_command){
    char* command=strtok(whole_command,"|");
    int num_command=1;

    int fd[2];
    int input=0;    //Initial input is stdin(0);
    int pid;

    while (command!=NULL){
        char* arguments[500];
        char* token=strtok(command, " \t\n");
        int i=0;
        while (token!=NULL){
            arguments[i++]=token;
            token=strtok(NULL, " \t\n");
        }
        arguments[i]=NULL;
        command=strtok(NULL,"|");                   //Next command to be executed.
        if (command!=NULL){
            if (pipe(fd)==-1){
            perror("Pipe creation failed");
            exit(1);
            }
        }
        pid=fork();
        if (pid<0){
            perror("Fork failed");
            exit(1);
        }
        if (pid==0){
            if (input!=0){                          //Stops unnecessary redirects as in first pipe it is already pointing to stdin
                if (dup2(input,STDIN_FILENO)==-1){
                perror("dup2 failed for the input");
                exit(1);
            }
            close(input);
        }
        
        if (command!=NULL){                         //Redirects output when not last command
            if (dup2(fd[1],STDOUT_FILENO)==-1){
                perror("dup2 failed for the output");
                exit(1);
            }
            close(fd[1]);
        }

        close(fd[0]);
        if (execvp(arguments[0], arguments) == -1) {
            perror("Command execution failed");
            exit(1);
        }

        if (input!= 0) {
            close(input);  // Close previous pipe's read end
        }
    
        close(fd[1]);  // Close the write end of the pipe in the parent
        input = fd[0];  // Set input_fd to the read end of the current pipe for the next command
    }

    
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}