#ifndef SIMPLE_MULTITHREADER_H
#define SIMPLE_MULTITHREADER_H

#include <pthread.h>
#include <stdlib.h>
#include <cstring>
#include <chrono>
#include <functional>

// Structure for thread arguments for 1D parallel_for
typedef struct {
    int low;            //start
    int high;           //end
    void* lambda;       //pointer to lambda function
} thread_args_1D;

// Structure for thread arguments for 2D parallel_for
typedef struct {
    int low1;           //start of outer for loop
    int high1;          //end of outer for loop
    int low2;           //start of inner for loop
    int high2;          //end of inner for loop
    void* lambda;       //pointer to lambda function
} thread_args_2D;

// starting point for 1D threads
void* thread_func_1D(void* arg) {
    thread_args_1D* data = (thread_args_1D*)arg;
    auto lambda = *reinterpret_cast<std::function<void(int)>*>(data->lambda);
    for (int i = data->low; i < data->high; ++i) {
        lambda(i);
    }
    return NULL;
}

// starting point for 2D threads
void* thread_func_2D(void* arg) {
    thread_args_2D* data = (thread_args_2D*)arg;
    auto lambda = *reinterpret_cast<std::function<void(int, int)>*>(data->lambda);
    for (int i = data->low1; i < data->high1; ++i) {
        for (int j = data->low2; j < data->high2; ++j) {
            lambda(i, j);
        }
    }
    return NULL;
}

// Parallel programming for 1D loop
void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
    auto start_time = std::chrono::high_resolution_clock::now();

    pthread_t tid[numThreads];
    thread_args_1D args[numThreads];
    //pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    //ThreadData1D* data = (ThreadData1D*)malloc(numThreads * sizeof(ThreadData1D));

    int range = high-low;
    int chunk = range/numThreads;
    int r=range%numThreads;
    chunk++;
    
    for (int i=0,j=0; i<numThreads; i++,j++){
        if (i==r){
            low+=(i*chunk);
            chunk--;
            j=0;
        }
        args[i].low=low+(j*chunk);
        args[i].high=((low+(j+1)*chunk) > high ? high: low+(j+1)*chunk);
        args[i].lambda=&lambda;
        int rc = pthread_create(&tid[i],NULL,thread_func_1D,(void*)&args[i]);
        if (rc){
            fprintf(stderr, "Error: pthread_create() failed for thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i=0; i<numThreads; i++){
        int rc = pthread_join(tid[i],NULL);
        if (rc){
            fprintf(stderr, "Error: pthread_join() failed for thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    printf("Execution time: %f seconds\n", elapsed.count());
}

// Parallel programming for 2D loop
void parallel_for(int low1, int high1, int low2, int high2, 
                  std::function<void(int, int)> &&lambda, int numThreads) {
    auto start_time = std::chrono::high_resolution_clock::now();

    pthread_t tid[numThreads];
    thread_args_2D args[numThreads];

    int range1 = high1-low1;
    int chunk = range1/numThreads;
    int r=range1%numThreads;
    chunk++;
    
    for (int i=0,j=0; i<numThreads; i++,j++){
        if (i==r){
            low1+=(i*chunk);
            chunk--;
            j=0;
        }
        args[i].low1=low1+(j*chunk);
        args[i].high1=((low1+(j+1)*chunk) > high1 ? high1: low1+(j+1)*chunk);
        args[i].low2=low2;
        args[i].high2=high2;
        args[i].lambda=&lambda;
        int rc = pthread_create(&tid[i],NULL,thread_func_2D,(void*)&args[i]);
        //if thread creation successfull rc=0;
        if (rc){
            fprintf(stderr, "Error: pthread_create() failed for thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i=0; i<numThreads; i++){
        int rc = pthread_join(tid[i],NULL);
        if (rc){
            fprintf(stderr, "Error: pthread_join() failed for thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    printf("Execution time: %f seconds\n", elapsed.count());
}

#endif // SIMPLE_MULTITHREADER_H
