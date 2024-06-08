#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define NUMBERS 5

void loadArray(int *array) {
    srand(time(NULL));
    for (int i = 0; i < NUMBERS; i++) {
        array[i] = rand() % 12 + 1;
    }
}

void printArray(const int *array) {
    for (int i = 0; i < NUMBERS; i++) {
        printf("%d\t", array[i]);
    }
    printf("\n");
}

int main(void) {
    int *numbers = calloc(NUMBERS, sizeof(int)), totalSum = 0;

    loadArray(numbers);

    printf("ARRAY\n");
    printArray(numbers);

    #pragma omp parallel shared(totalSum)
    {
        const int threadCount = omp_get_num_threads();
        int operationsPerCore = NUMBERS / threadCount;
        const int threadIndex = omp_get_thread_num();
        int partialSum = 0;

        const int rest = NUMBERS % threadCount;
        int step;

        if (threadIndex < rest) {
            operationsPerCore++;
            step = 0;
        } else {
            step = rest;
        }

        for (int i = 0; i < operationsPerCore; i++) {
            partialSum += numbers[threadIndex * operationsPerCore + i + step];
        }

        totalSum += partialSum;
    }

    printf("\nTOTAL SUM = %d\n", totalSum);

    free(numbers);

    return 0;
}
