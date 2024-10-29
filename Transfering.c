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

// void display(Queue *q) {
//     if (empty(q)) {
//         printf("EMPTY\n");
//         return;
//     }
//     Job_PCB* temp = q->front;
//     printf("QUEUE --> ");
//     while (temp != NULL) {
//         printf(" %d ->", temp->pid);
//         temp = temp->next;
//     }
//     printf("\n");
// }

int size(Queue *q){
    return q->size;
}

Job_PCB terminated[1000];
int terminated_count=0;
int NCPU;
float TSLICE;
Queue running_queue;
Queue ready_queue;


void context_switch(){
    //code for the running and ready queue thing.
    if (ready_count==0 && running_count==0){        //if no processes have arrived or running.
        return;
    }//fetch load
    int status;
    int rc = running_count;
    for (int i = 0; i < rc; i++){
        Job_PCB* j = dequeue(&running_queue);
        running_count--;
        int result=waitpid(j->pid, &status, WNOHANG);
        if (result == 0){
            kill(j->pid, SIGSTOP);
            //wait time.
            timespec cur_time;
            clock_gettime(CLOCK_MONOTONIC, &cur_time);
            j->wait_time.tv_sec+=(cur_time.tv_sec-j->prev_time.tv_sec);
            j->wait_time.tv_nsec+=(cur_time.tv_nsec-j->prev_time.tv_nsec);
            if (j->wait_time.tv_nsec<0){
                j->wait_time.tv_sec--;
                j->wait_time.tv_nsec+=1000000000;
            }
            enqueue(&ready_queue,j);
            ready_count++;
        }
        else if (result == -1){
                    // printf("completed %d\n", p->completed);
                    if (j->completed != 1){
                        j->completed = 1;
                        // printf("%d %s equeued 3\n", p->pid,p->command[0]);
                        enqueue(&ready_queue,j);
                        ready_count++;
                    }
        }
        else{
            clock_gettime(CLOCK_MONOTONIC,&j->end_time);
            timespec cur_time;
            clock_gettime(CLOCK_MONOTONIC, &cur_time);
            j->wait_time.tv_sec+=(cur_time.tv_sec-j->prev_time.tv_sec);
            j->wait_time.tv_nsec+=(cur_time.tv_nsec-j->prev_time.tv_nsec);
            if (j->wait_time.tv_nsec<0){
                j->wait_time.tv_sec--;
                j->wait_time.tv_nsec+=1000000000;
            }
            if (terminated_count>=1000){
                perror("Memory assigned for terminated processes full");
                continue;
            }
            terminated[terminated_count++]=*j;
        }
    }

    int rdc = ready_count;
    for (int i = 0; i < NCPU && i < rdc; i++){
        Job_PCB* j = dequeue(&ready_queue);
        enqueue(&running_queue, j);
        ready_count--;
        running_count++;
        kill(j->pid, SIGUSR1);   //For starting execution if not started.
        kill(j->pid, SIGCONT); 
    }
}

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
            exit(0);
        }
        else {
            j.pid = pid;
            clock_gettime(CLOCK_MONOTONIC,&j.start_time);
            clock_gettime(CLOCK_MONOTONIC,&j.prev_time);
            enqueue(&ready_queue, &j);
            ready_count++;
            printf("Ready[index].job_name: %s\n",j.job_name);
        }
    }
    else if (signum==SIGINT){
        printf("SIGINT recieved Scheduler");
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
    }
}

int main(int argc, char* argv[]){

     struct sigaction ssh;
    memset(&ssh, 0, sizeof(ssh));
    ssh.sa_handler = scheduler_signal_handler;
    sigaction(SIGUSR2, &ssh, NULL);
    sigaction(SIGINT, &ssh, NULL);
    sigemptyset(&ssh.sa_mask); 
    ssh.sa_flags = 0;

    NCPU = atoi(argv[1]);
    TSLICE = strtof(argv[2],NULL);

    while (1){
        unsigned int sleep_time=TSLICE;
        while (sleep_time>0){
            sleep_time=usleep(sleep_time);       //if signal comes the sleep will continue.
        }
        context_switch();
    }
    exit(1);

}
