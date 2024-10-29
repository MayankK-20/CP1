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

typedef struct timespec timespec;


/// QUEUE


typedef struct Job_PCB{
    pid_t pid;
    char* job_name;
    timespec wait_time;
    timespec start_time;
    timespec end_time;
    timespec prev_time;
    int priority;
    int completed;
    struct Job_PCB* next;
    //completion time = end_time - start_time
} Job_PCB;


typedef struct Queue{
    Job_PCB *front;
    Job_PCB *back;
    int size;
} Queue;

void queue(Queue *q) {
    q->front = NULL;
    q->back = NULL;
    q->size = 0;
}

int empty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue* q, Job_PCB* p) {
    //clock_gettime(CLOCK_REALTIME, &p->eq_time);
    p->next = NULL;
    if (empty(q)) {
        q->front = p;
        q->back = p;
    } else {
        q->back->next = p;
        q->back = p;
    }
    q->size++;
}

Job_PCB* dequeue(Queue *q) {
    if (empty(q)) {
        printf("QUEUE IS EMPTY\n");
        return NULL;
    }
    Job_PCB* temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->back = NULL;
    }
    temp->next = NULL;
    q->size--;
    return temp;
}

void display(Queue *q) {
    if (empty(q)) {
        printf("EMPTY\n");
        return;
    }
    Job_PCB* temp = q->front;
    printf("QUEUE --> ");
    while (temp != NULL) {
        printf(" %d ->", temp->pid);
        temp = temp->next;
    }
    printf("\n");
}

int size(Queue *q){
    return q->size;
}

typedef struct shm_t{
    atomic_int ready_count;
    atomic_int running_count;
} shm_t;

// Job_PCB ready[1000];
// Job_PCB running[1000];
Job_PCB terminated[1000];
int terminated_count;
int NCPU;
float TSLICE;
pid_t scheduler_pid;
int shm_fd;
shm_t* shared_mem;
int pipefd[2];
Queue running_queue;
Queue ready_queue;

int launch(char* command){
    if (strcmp(command,"\n")==0 ||command==NULL || strlen(command)==0){
        return 1;
    }
    char* comm = strdup(command);
    if (comm==NULL){
        perror("strdup failed");
        return 1;
    }
    char* arguments[500];
    char* token = strtok(command, " \t\n");                                                              //flag for background command &
    int i = 0;
    while (token != NULL) {
        arguments[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    arguments[i] = NULL;
    if (strcmp(arguments[0],"submit")==0){
        write(pipefd[1], comm, strlen(comm) + 1); //writing to pipe.
        kill(scheduler_pid, SIGUSR2);
        return 1;
    }
    //if command not submit.
    int pid=fork();
    if (pid<0){
        perror("Fork Failed");
    }
    else if (pid==0){
        if (execvp(arguments[0], arguments)==-1){
            //execvp searches for executable replaces it with the child process if succesfull it will never return else return -1;
            perror("Command could not be executed");
            return 1;
        }
    }
    else{
        waitpid(pid,NULL,0);
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
    status = launch(command);
    free(command);
  } while (status);
}

void context_switch(){
    //code for the running and ready queue thing.
    if (shared_mem->ready_count==0 && shared_mem->running_count==0){        //if no processes have arrived or running.
        return;
    }//fetch load
    int status;
    int rc = shared_mem->running_count;
    for (int i = 0; i < rc; i++){
        Job_PCB* j = dequeue(&running_queue);
        atomic_fetch_sub(&shared_mem->running_count,1); 
        int result=waitpid(j->pid, &status, WNOHANG);

        if (result == 0){
            kill(j->pid, SIGSTOP);
            //wait time.
            timespec cur_time;
            clock_gettime(CLOCK_MONOTONIC, &cur_time);
            j.wait_time.tv_sec+=(cur_time.tv_sec-j.prev_time.tv_sec);
            j.wait_time.tv_nsec+=(cur_time.tv_nsec-j.prev_time.tv_nsec);
            if (j.wait_time.tv_nsec<0){
                j.wait_time.tv_sec--;
                j.wait_time.tv_nsec+=1000000000;
            }
            int index=atomic_fetch_add(&shared_mem->ready_count,1); 

            //gives previous value of ready_count and used to handle race condition.
            if (index>=1000){
                perror("Ready Queue Full");
                atomic_fetch_sub(&shared_mem->ready_count,1);
                free(j->job_name);
                continue;
            }
            enqueue(&ready_queue,j);
        }
        else if (result == -1){
                    // printf("completed %d\n", p->completed);
                    if (j->completed != 1){
                        j->completed = 1;
                        // printf("%d %s equeued 3\n", p->pid,p->command[0]);
                        terminated[terminated_count++]=*j;
                    }
        }
        else{
            clock_gettime(CLOCK_MONOTONIC,&j->end_time);
            timespec cur_time;
            clock_gettime(CLOCK_MONOTONIC, &cur_time);
            j.wait_time.tv_sec+=(cur_time.tv_sec-j.prev_time.tv_sec);
            j.wait_time.tv_nsec+=(cur_time.tv_nsec-j.prev_time.tv_nsec);
            if (j.wait_time.tv_nsec<0){
                j.wait_time.tv_sec--;
                j.wait_time.tv_nsec+=1000000000;
            }
            if (terminated_count>=1000){
                perror("Memory assigned for terminated processes full");
                continue;
            }
            terminated[terminated_count++]=*j;
        }
    }

    int rdc = shared_mem->ready_count;
    for (int i = 0; i < NCPU && i < rdc; i++){
        Job_PCB* j = dequeue(&ready_queue);
        enqueue(&running_queue, j);
        atomic_fetch_sub(&shared_mem->ready_count,1);
        atomic_fetch_add(&shared_mem->running_count,1);
        kill(j->pid, SIGUSR1);   //For starting execution if not started.
        kill(j->pid, SIGCONT); 
        
    }
}

static void shell_signal_handler(int signum) {                        //********************************Do with write
    if (signum == SIGINT){
        printf("Recieved SIGINT");
        //wait for all user processes to be executed.
        while (shared_mem->ready_count>0 || shared_mem->running_count>0){
            printf("All Jobs not completed yet");
            sleep(TSLICE);
        }
        kill(scheduler_pid, SIGINT);
        munmap(shared_mem, sizeof(shm_t));
        shm_unlink("/shm_struct");
        close(shm_fd);
        waitpid(scheduler_pid,NULL,0);
        exit(0);
    }
}
//change.
void scheduler_signal_handler(int signum){
    if (signum==SIGUSR2){
        printf("SIGUSR2 received\n");
        char buffer[512];
        read(pipefd[0], buffer, sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        Job_PCB j;
        j.job_name=strdup(buffer);
        if (j.job_name==NULL){
            perror("strdup failed");
            return;
        }
        j.wait_time.tv_sec=0;
        j.wait_time.tv_nsec=0;
        char* arguments[500];
        char* token = strtok(buffer, " \t\n");
        int i = 0;
        while (token != NULL) {
            arguments[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        arguments[i] = NULL;
        int pid=fork();
        if (pid<0){
            perror("Fork Failed");
        }
        else if (pid==0){
            char* args[]={arguments[1],NULL};
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
            enqueue(&ready_queue, &j);
            printf("Ready[index].job_name: %s\n",j.job_name);
        }
    }
    else if (signum==SIGINT){
        printf("SIGINT recieved");
        //print the name, pid, completion time, and wait time of all the jobs submitted by the user and exit gracefully.
        for (int i=0; i<terminated_count; i++){
            Job_PCB j=terminated[i];
            printf("Command: %s\n",j.job_name);
            printf("PID: %d\n",j.pid);
            printf("Wait time: %ld seconds, %ld nanoseconds\n", j.wait_time.tv_sec, j.wait_time.tv_nsec);
            timespec completion={0,0};
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
        munmap(shared_mem, sizeof(shm_t));
    }
}

void create_shared_memory(){
    shm_unlink("/shm_struct"); //remove if anything earlier.
    //o_creat = create if not there. o_rdwr = read and write, 0666 = wide access for owner, group and others. read and write permission mainly.
    shm_fd = shm_open("/shm_struct", O_CREAT | O_RDWR, 0666);       //file descriptor.
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    // Set size of shared memory
    if (ftruncate(shm_fd, sizeof(shm_t)) == -1) {
        perror("ftruncate failed");
        close(shm_fd);
        exit(1);
    }

    // Map shared memory
    shared_mem = mmap(NULL, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        exit(1);
    }

    //Initializing
    memset(shared_mem, 0, sizeof(shm_t));    
    atomic_init(&shared_mem->ready_count, 0);
    atomic_init(&shared_mem->running_count,0);
}

int main(int argc, char* argv[]){
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }
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
    TSLICE = strtof(argv[2],NULL);

    if (NCPU<=0 || TSLICE<=0.0){
        perror("invalid ncpu or tslice values");
        exit(1);
    }

    create_shared_memory();
    printf("created shared memory");

    scheduler_pid = fork();
    if (scheduler_pid < 0){
        perror("fork failed!");
        exit(1);
    }
    else if (scheduler_pid == 0){
        struct sigaction ssh;
        memset(&ssh, 0, sizeof(ssh));
        ssh.sa_handler = scheduler_signal_handler;
        sigaction(SIGUSR2, &ssh, NULL);
        sigaction(SIGINT, &ssh, NULL);
        printf("TSLICE: %f", TSLICE);
        fflush(stdout);
        while (1){
            unsigned int sleep_time=TSLICE;
            while (sleep_time>0){
                sleep_time=usleep(sleep_time);       //if signal comes the sleep will continue.
            }
            context_switch();
        }
        exit(1);
    }
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = shell_signal_handler;
    sigaction(SIGINT, &sig, NULL);
    shell_loop();
    return 0;
}
