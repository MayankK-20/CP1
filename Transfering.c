#ifndef SIMPLE_MULTITHREADER_H
#define SIMPLE_MULTITHREADER_H

#include <pthread.h>
#include <stdlib.h>
#include <cstring>
#include <chrono>
#include <stdio.h>
#include <list>
#include <functional>


// Structure for thread arguments for 2D parallel_for
typedef struct {
    int low1;           //start of outer for loop
    int high1;          //end of outer for loop
    int low2;           //start of inner for loop
    int high2;          //end of inner for loop
    void* lambda;       //pointer to lambda function
} thread_args;

// starting point for 1D threads
void* thread_func(void* arg) {
    thread_args* data = (thread_args*)arg;
    if (data->low2 < 0){
        auto lambda = *reinterpret_cast<std::function<void(int)>*>(data->lambda);       //as the lambda passed into this function is void* we recast it.
        for (int i = data->low1; i < data->high1; ++i) {
            lambda(i);
        }
    }
    else{
        auto lambda = *reinterpret_cast<std::function<void(int, int)>*>(data->lambda);  //as the lambda passed into this function is void* we recast it.
        for (int i = data->low1; i < data->high1; ++i) {
            for (int j = data->low2; j < data->high2; ++j) {
                lambda(i, j);
            }
        }
    }

    return NULL;
}

// Parallel programming for 1D loop
void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
    numThreads --;
    auto start_time = std::chrono::high_resolution_clock::now();

    pthread_t tid[numThreads];
    thread_args args[numThreads];
    
    int range = high-low;
    int chunk = range/numThreads;
    int r=range%numThreads;
    chunk++;
    
    //Below for loop gives first r functions chunk+1 size and others chunk size to optimally handle remainders.
    for (int i=0,j=0; i<numThreads; i++,j++){
        if (i==r){
            low+=(i*chunk);
            chunk--;
            j=0;
        }
        args[i].low1=low+(j*chunk);
        args[i].high1=((low+(j+1)*chunk) > high ? high: low+(j+1)*chunk);
        args[i].low2=-1;
        args[i].high2=-1;
        args[i].lambda=&lambda;
        int rc = pthread_create(&tid[i],NULL,thread_func,(void*)&args[i]);
        if (rc){
            fprintf(stderr, "Error: pthread_create() failed for thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    //waiting for threads to terminate.
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
void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads) {
    numThreads --;
    auto start_time = std::chrono::high_resolution_clock::now();

    pthread_t tid[numThreads];
    thread_args args[numThreads];

    int range1 = high1-low1;
    int chunk = range1/numThreads;
    int r=range1%numThreads;
    chunk++;
    
    //Below for loop gives first r functions chunk+1 size and others chunk size to optimally handle remainders.
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
        int rc = pthread_create(&tid[i],NULL,thread_func,(void*)&args[i]);
        //if thread creation successfull rc=0;
        if (rc){
            fprintf(stderr, "Error: pthread_create() failed for thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    //waiting for threads to terminate.
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

#endif
