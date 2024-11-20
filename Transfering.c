#ifndef SIMPLE_MULTITHREADER_H
#define SIMPLE_MULTITHREADER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <functional>

// Structure for passing thread data for 1D parallel_for
typedef struct {
    int low;
    int high;
    void* lambda; // Pointer to the C++ lambda function
} ThreadData1D;

// Structure for passing thread data for 2D parallel_for
typedef struct {
    int low1;
    int high1;
    int low2;
    int high2;
    void* lambda; // Pointer to the C++ lambda function
} ThreadData2D;

// Thread function for 1D parallel_for
void* thread_func_1D(void* arg) {
    ThreadData1D* data = (ThreadData1D*)arg;
    auto lambda = *reinterpret_cast<std::function<void(int)>*>(data->lambda);
    for (int i = data->low; i < data->high; ++i) {
        lambda(i);
    }
    return NULL;
}

// Thread function for 2D parallel_for
void* thread_func_2D(void* arg) {
    ThreadData2D* data = (ThreadData2D*)arg;
    auto lambda = *reinterpret_cast<std::function<void(int, int)>*>(data->lambda);
    for (int i = data->low1; i < data->high1; ++i) {
        for (int j = data->low2; j < data->high2; ++j) {
            lambda(i, j);
        }
    }
    return NULL;
}

// Parallelize 1D loop
void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
    auto start_time = std::chrono::high_resolution_clock::now();

    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    ThreadData1D* data = (ThreadData1D*)malloc(numThreads * sizeof(ThreadData1D));

    int range = high - low;
    int chunkSize = (range + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; ++t) {
        data[t].low = low + t * chunkSize;
        data[t].high = (low + (t + 1) * chunkSize > high) ? high : low + (t + 1) * chunkSize;
        data[t].lambda = &lambda;
        int rc = pthread_create(&threads[t], NULL, thread_func_1D, &data[t]);
        if (rc) {
            fprintf(stderr, "Error: pthread_create() failed for thread %d\n", t);
            exit(EXIT_FAILURE);
        }
    }

    for (int t = 0; t < numThreads; ++t) {
        pthread_join(threads[t], NULL);
    }

    free(threads);
    free(data);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    printf("Execution time: %f seconds\n", elapsed.count());
}

// Parallelize 2D loop
void parallel_for(int low1, int high1, int low2, int high2, 
                  std::function<void(int, int)> &&lambda, int numThreads) {
    auto start_time = std::chrono::high_resolution_clock::now();

    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    ThreadData2D* data = (ThreadData2D*)malloc(numThreads * sizeof(ThreadData2D));

    int range1 = high1 - low1;
    int chunkSize = (range1 + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; ++t) {
        data[t].low1 = low1 + t * chunkSize;
        data[t].high1 = (low1 + (t + 1) * chunkSize > high1) ? high1 : low1 + (t + 1) * chunkSize;
        data[t].low2 = low2;
        data[t].high2 = high2;
        data[t].lambda = &lambda;
        int rc = pthread_create(&threads[t], NULL, thread_func_2D, &data[t]);
        if (rc) {
            fprintf(stderr, "Error: pthread_create() failed for thread %d\n", t);
            exit(EXIT_FAILURE);
        }
    }

    for (int t = 0; t < numThreads; ++t) {
        pthread_join(threads[t], NULL);
    }

    free(threads);
    free(data);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    printf("Execution time: %f seconds\n", elapsed.count());
}

#endif // SIMPLE_MULTITHREADER_H
