#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>
#include<sys/mman.h>    //mmap and munmap
#include<stdatomic.h>
#include<fcntl.h>       //shm_open and ftruncate
#include<signal.h>

//***************************give error handling for all sys calls. */

typedef struct timespec timespec;

typedef struct Job_PCB{
    pid_t pid;
    char* job_name;
    timespec wait_time;
    timespec start_time;
    timespec end_time;
    timespec prev_time;
    int priority;
    //completion time = end_time - start_time
} Job_PCB;

typedef struct shm_t{
    Job_PCB ready[1000];
    atomic_int ready_count;
    Job_PCB running[1000];
    int running_count;
    Job_PCB terminated[1000];
    int terminated_count;
    Job_PCB new_job;
} shm_t;


int NCPU;
float TSLICE;
char* name;
pid_t scheduler_pid;
int shm_fd;
shm_t* shared_mem;

int launch2(char** args, int bg){
    if (strcmp(args[0],"submit")==0){
        shared_mem->new_job.job_name = strdup(args[1]);
        if (shared_mem->new_job.job_name == NULL){
            perror("strdup failed");
            return 1;
        }
        if (args[2]!=NULL){
            shared_mem->new_job.priority = atoi(args[2]);
        }
        kill(scheduler_pid, SIGUSR2);
        return 1;
    }
    int pid=fork();
    if (pid<0){
        perror("Fork Failed");
    }
    else if (pid==0){
        if (execvp(args[0], args)==-1){
            //execvp searches for executable replaces it with the child process if succesfull it will never return else return -1;
            perror("Command could not be executed");
            return 1;
        }
    }
    else {
        if (!bg){
            waitpid(pid,NULL,0);
        }
    }
    return 1;
}

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

    for (int j=0; j<num_commands; j++) {

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

        if (j==0 && 1==num_commands){
            return launch2(arguments, bg);
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

void shell_loop(){
  int status;
  do{
    printf("shell: ");
    char* command = read_user_input();
    name=strdup(command);
    status = launch(command);
    free(command);
  } while (status);
}

void context_switch(){
    //code for the running and ready queue thing.
    if (shared_mem->ready_count==0 && shared_mem->running_count==0){        //if no processes have arrived or running.
        return;
    }

    //Removing processes from running queue;
    for (int i=0; i<shared_mem->running_count; i++){
        Job_PCB j=shared_mem->running[i];
        int status;
        int result=waitpid(j.pid, &status, WNOHANG);
        if (result==-1){
            printf("%d", j.pid);
            perror("Error checking job status");
            continue;
        }
        timespec cur_time;
        clock_gettime(CLOCK_MONOTONIC, &cur_time);
        j.wait_time.tv_sec+=(cur_time.tv_sec-j.prev_time.tv_sec);
        j.wait_time.tv_nsec+=(cur_time.tv_nsec-j.prev_time.tv_nsec);
        if (j.wait_time.tv_nsec<0){
            j.wait_time.tv_sec--;
            j.wait_time.tv_nsec+=1000000000;
        }
        if (result==0){             //Job not finished.
            kill(j.pid, SIGSTOP);
            //wait time.
            int index=atomic_fetch_add(&shared_mem->ready_count,1);         //gives previous value of ready_count and used to handle race condition.
            if (index>=1000){
                perror("Ready Queue Full");
                atomic_fetch_sub(&shared_mem->ready_count,1);
                free(j.job_name);
                continue;
            }
            shared_mem->ready[index]=j;
        }
        else{               //Job finished.
            clock_gettime(CLOCK_MONOTONIC,&j.end_time);
            if (shared_mem->terminated_count>=1000){
                perror("Memory assigned for terminated processes full");
                continue;
            }
            shared_mem->terminated[shared_mem->terminated_count++]=j;
        }
    }

    //Running processes                                                         //*********Think about the race condition. */
    int to_run=(NCPU<shared_mem->ready_count? NCPU: shared_mem->ready_count);
    for (int i=0; i<to_run; i++){
        Job_PCB j = shared_mem->ready[i];
        kill(j.pid, SIGUSR1);   //For starting execution if not started.
        kill(j.pid, SIGCONT); // Continue the job
        shared_mem->running[shared_mem->running_count++] = j;
    }

    for (int i=0; i<shared_mem->ready_count-to_run; i++){
        shared_mem->ready[i]=shared_mem->ready[i+to_run];
    }

    atomic_fetch_sub(&shared_mem->ready_count,to_run);
}


static void my_handler(int signum) {                        //********************************Do with write
    if (signum == SIGINT){
        //wait for all user processes to be executed.
        while (shared_mem->ready_count>0 || shared_mem->running_count>0){
            sleep(TSLICE);
        }
        //print the name, pid, completion time, and wait time of all the jobs submitted by the user and exit gracefully.
        for (int i=0; i<shared_mem->terminated_count; i++){
            Job_PCB j=shared_mem->terminated[i];
            printf("Command: %d\n",j.job_name);
            printf("PID: %d\n",j.pid);
            printf("Wait time: %ld seconds, %ld nanoseconds\n", j.wait_time.tv_sec, j.wait_time.tv_nsec);
            timespec completion;
            completion.tv_sec+=(j.end_time.tv_sec-j.start_time.tv_sec);
            completion.tv_nsec+=(j.end_time.tv_nsec-j.start_time.tv_nsec);
            if (completion.tv_nsec<0){
                completion.tv_sec--;
                completion.tv_nsec+=1000000000;
            }
            printf("Completion time: %ld seconds, %ld nanoseconds\n", completion.tv_sec,completion.tv_nsec);
            printf("\n");
            free(j.job_name);
        }
        free(name);
        kill(scheduler_pid, SIGKILL);
        munmap(shared_mem, sizeof(shm_t));
        shm_unlink("/shm_struct");
        close(shm_fd);
        exit(0);
    }
    if (signum==SIGUSR2){
        Job_PCB j;
        j.job_name=strdup(shared_mem->new_job.job_name);
        j.wait_time.tv_sec=0;
        j.wait_time.tv_nsec=0;
        j.priority=shared_mem->new_job.priority;
        int pid=fork();
        if (pid<0){
            perror("Fork Failed");
        }
        else if (pid==0){
            char** args={j.job_name, NULL};
            if (execvp(args[0], args)==-1){
                perror("Command could not be executed");
            }
        }
        else {
            j.pid = pid;
            clock_gettime(CLOCK_MONOTONIC,&j.start_time);
            clock_gettime(CLOCK_MONOTONIC,&j.prev_time);
            int index=atomic_fetch_add(&shared_mem->ready_count,1);         //gives previous value of ready_count and used to handle race condition.
            if (index>=1000){
                perror("Ready Queue Full");
                atomic_fetch_sub(&shared_mem->ready_count,1);
            }
            shared_mem->ready[index]=j;
        }
    }
}


int main(int argc, char* argv[]){
    //error handling if ncpu or tslice not provided
    if (argc != 3){
        perror("insufficient command line arguments provided");
        exit(1);
    }

    //error checking if ncpu or tslice is not number
    // if (!is_numeric(argv[1] || !is_numeric(argv[2]){
    //     perror("ncpu and tslice must be positive numbers");
    //     exit(1);
    // }

    // ncpu and tslice cannot have leading + as well (can have whitespace leading)
    NCPU = atoi(argv[1]);
    TSLICE = strtof(argv[2],NULL)/10;

    if (NCPU<=0 || TSLICE<=0.0){
        perror("invalid ncpu or tslice values");
        exit(1);
    }
    
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = my_handler;
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGUSR2, &sig, NULL);
//o_creat = create if not there. o_rdwr = read and write, 0666 = wide access for owner, group and others. read and write permission mainly.
    shm_fd = shm_open("/shm_struct", O_CREAT | O_RDWR, 0666);       //file descriptor.
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, sizeof(shm_t)) == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    // Map shared memory
    shared_mem = mmap(NULL, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    //Initializing
    atomic_init(&shared_mem->ready_count, 0);
    shared_mem->running_count=0;
    shared_mem->terminated_count=0;

    scheduler_pid = fork();
    if (scheduler_pid < 0){
        perror("fork failed!");
        exit(1);
    }
    else if (scheduler_pid == 0){
        while (1){
            unsigned int sleep_time=TSLICE;
            while (sleep_time>0){
                sleep_time=sleep(sleep_time);       //if signal comes the sleep will continue.
            }
            context_switch();
        }
        exit(1);
    }
    shell_loop();
    return 0;
}
