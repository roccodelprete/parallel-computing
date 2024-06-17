#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

/**
 *  Implementare  un  programma  parallelo  per 
 * l’ambiente  multicore  con  np  unità  processanti  che 
 * impieghi la libreria OpenMP. Il programma deve essere 
 * organizzato come segue: il core master deve leggere un 
 * vettore a, di dimensione N ed uno scalare beta. Quindi 
 * i core devo collaborare per verificare se nel vettore 
 * esista almeno un elemento uguale al valore beta.  
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

void findValueIntoArray(const int *array, const int size, const int beta, const int numThreads) {
    int i = 0, found = 0;

    const double startTime = omp_get_wtime();

    #pragma omp parallel for private(i) shared(array, size, beta, found) num_threads(numThreads)
    for (i = 0; i < size; i++) {
        if (array[i] == beta) {
            found = 1;
        }
    }

    const double endTime = omp_get_wtime();

    if (found) {
        printf("%d found into array\n", beta);
    } else {
        printf("%d not found into array\n", beta);
    }

    printf("\nExecution time = %lf\n", getExecutionTime(startTime, endTime));

    free(array);
}

int main(void) {
    int size, numThreads, beta;
    printf("Insert array size: ");
    scanf("%d", &size);
    printf("\n");

    int *array = arrayInit(size);
    printf("\nArray: \n");
    printArray(array, size);
    printf("\n");

    printf("Insert beta value: ");
    scanf("%d", &beta);
    printf("\n");

    printf("Insert threads number: ");
    scanf("%d", &numThreads);
    printf("\n");

    findValueIntoArray(array, size, beta, numThreads);

    return 0;
}
