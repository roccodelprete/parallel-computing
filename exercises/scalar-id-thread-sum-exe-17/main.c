#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

/**
 *  Implementare  un  programma  parallelo  per 
 * l’ambiente  multicore  con  np  unità  processanti  che 
 * impieghi la libreria OpenMP. Il programma deve essere 
 * organizzato come segue: il core master deve leggere 
 * quattro valori scalari a0, a1, a2, a3, quindi i core devono 
 * collaborare per costruire un vettore di dimensione 4np 
 * i cui elementi sono dati dalla somma di tali valori con il 
 * proprio identificativo.
 */

int *arrayAllocation(const int size) {
    return calloc(size, sizeof(int));
}

int *arrayInit(const int size) {
    int *array = calloc(size, sizeof(int));

    for (int i = 0; i < size; i++) {
        printf("array[%d] = ", i + 1);
        scanf("%d", array + i);
    }

    return array;
}

void printArray(const int *array, const int size) {
    for (int i = 0; i < size; i++) {
        printf("%d\t", array[i]);
    }

    printf("\n");
}

double getExecutionTime(const double start, const double end) {
    return end - start;
}

void findValueIntoArray(const int *alphas, const int numThreads) {
    const int resultArraySize = 4 * numThreads;
    int *result = calloc(resultArraySize, sizeof(int));
    int i = 0;

    const double startTime = omp_get_wtime();

    #pragma omp parallel private(i) shared(alphas, result, resultArraySize) num_threads(numThreads)
    {
        const int threadId = omp_get_thread_num();
        const int value = 4 * threadId;

        for (i = 0; i < 4; i++) {
            result[i + value] = alphas[i] + threadId;
        }
    }

    const double endTime = omp_get_wtime();

    printf("Result array: \n");
    printArray(result, resultArraySize);
    printf("\n");

    printf("Execution time = %lf\n", getExecutionTime(startTime, endTime));

    free(result);
    free(alphas);
}

int main(void) {
    int *alphas = arrayInit(4);
    printf("\nValues: \n");
    printArray(alphas, 4);
    printf("\n");

    int numThreads;
    printf("Insert threads number: ");
    scanf("%d", &numThreads);
    printf("\n");

    findValueIntoArray(alphas, numThreads);

    return 0;
}
