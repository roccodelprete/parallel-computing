#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
 *  Implementazione di un algoritmo parallelo (np core)
 *  per il calcolo dell’operazione c=alpha*a+b, con a,b
 *  vettori di dimensione N e alpha uno scalare, in
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

int *scalarPerArrays(const int *firstArray, const int *secondArray, const int size, const int scalar) {
    int *result = calloc(size, sizeof(int));
    int i = 0;

    const double startTime = omp_get_wtime();

#pragma omp parallel for private(i) shared(firstArray, secondArray, result, scalar, size)
    for (i = 0; i < size; i++) {
        result[i] = scalar * firstArray[i] + secondArray[i];
    }

    const double endTime = omp_get_wtime();

    printf("Execution time = %lf\n", getExecutionTime(startTime, endTime));

    return result;
}

int main(void) {
    srand(time(NULL));

    int size;
    printf("Enter the size of the array: ");
    scanf("%d", &size);

    int *firstArray = calloc(size, sizeof(int));
    int *secondArray = calloc(size, sizeof(int));

    fillArray(firstArray, size);
    fillArray(secondArray, size);

    printf("First array \n");
    printArray(firstArray, size);

    printf("Second array \n");
    printArray(secondArray, size);

    int alpha;
    printf("Enter the scalar: ");
    scanf("%d", &alpha);

    int *result = scalarPerArrays(firstArray, secondArray, size, alpha);

    printf("Result array \n");
    printArray(result, size);

    free(firstArray);
    free(secondArray);
    free(result);

    return 0;
}
