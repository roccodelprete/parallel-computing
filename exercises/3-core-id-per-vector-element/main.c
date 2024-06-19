#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
*  Implementazione di un algoritmo parallelo (np core)
*  per il calcolo degli elementi di un vettore c, i cui valori
*  sono ottenuti moltiplicando l’identificativo del core per
*  gli elementi di un vettore a di dimensione N, in
*  ambiente openMP
*/

void fillArray(int *array, const int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 1 + rand() % 10;
    }
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

void scalarPerArrays(const int *array, const int size, const int numThreads) {
    int *result = calloc(size, sizeof(int));
    int i = 0;

    const double startTime = omp_get_wtime();

    #pragma omp parallel for private(i) shared(array, result, size) num_threads(numThreads)

    for (i = 0; i < size; i++) {
        result[i] = omp_get_thread_num() * array[i];
    }

    const double endTime = omp_get_wtime();

    printf("\nResult array \n");
    printArray(result, size);

    printf("\nExecution time = %lf\n", getExecutionTime(startTime, endTime));

    free(result);
}

int main(void) {
    srand(time(NULL));

    int numThreads;
    printf("Enter the number of threads: ");
    scanf("%d", &numThreads);

    int size;
    printf("\nEnter the size of the array: ");
    scanf("%d", &size);

    int *array = calloc(size, sizeof(int));

    fillArray(array, size);

    printf("\nArray \n");
    printArray(array, size);

    scalarPerArrays(array, size, numThreads);

    free(array);

    return 0;
}
