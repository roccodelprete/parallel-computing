#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 6

void freeArrays(float *firstArray, float *secondArray, float *resultArray) {
    free(firstArray);
    free(secondArray);
    free(resultArray);
}

void printArray(float *array) {
    for (int i = 0; i < SIZE; i++) {
        printf("%f\t", array[i]);
    }
    printf("\n");
}

void loadArray(float *array) {
    for (int i = 0; i < SIZE; i++) {
        array[i] = i;
    }
}

int main(void) {
    float *firstArray = calloc(SIZE, sizeof(float));
    float *secondArray = calloc(SIZE, sizeof(float));
    float *resultArray = calloc(SIZE, sizeof(float));

    /**
     * @param operationsPerThread - number of operations per thread
     */
    int operationsPerThread;

    /**
     * @param remainingOperations - number of operations that are left after dividing the array into equal parts
     */
    int remainingOperations;

    /**
     * @param index - index of the element in the array
     */
    int index;

    /**
     * @param step - step for the index
     */
    int step;

    loadArray(firstArray);
    loadArray(secondArray);

    printf("FIRST ARRAY\n");
    printArray(firstArray);

    printf("\nSECOND ARRAY\n");
    printArray(secondArray);

#pragma omp parallel private(operationsPerThread, remainingOperations, index, step) shared(firstArray, secondArray, resultArray)
    {
        /**
         * @param thredaId - thread id
         */
        const int threadId = omp_get_thread_num();

        /**
         * @param numThreads - number of threads
         */
        const int numThreads = omp_get_num_threads();

        operationsPerThread = SIZE / numThreads;
        remainingOperations = SIZE % numThreads;

        if (threadId < remainingOperations) {
            operationsPerThread++;
            step = 0;
        } else {
            step = remainingOperations;
        }

        for (int i = 0; i < operationsPerThread; i++) {
            index = i + operationsPerThread * threadId + step;
            resultArray[index] = firstArray[index] + secondArray[index];
        }
    }

    printf("\nRESULT ARRAY\n");
    printArray(resultArray);

    freeArrays(firstArray, secondArray, resultArray);
}
