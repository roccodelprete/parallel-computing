#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
* Implementare un programma parallelo per
* l’ambiente multicore con np unità processanti che
* impieghi la libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master generare una
* matrice di dimensione N×M. Quindi, ogni core deve
* estrarre la riga della matrice con indice uguale al
* proprio identificativo e conservarla in un vettore locale;
* successivamente deve sommare gli elementi di tale
* vettore e conservare il risultato in un vettore c di
* dimensione np. Infine, i core devo collaborare per
* individuare in massimo tra gli elementi del vettore c.
*/

struct returnType {
    int maxValue;
    double sumRegionTime;
    double maxRegionTime;
};

int *arrayAllocation(const int size) {
    int *vector = calloc(size, sizeof(int));

    return vector;
}

int **allocateMatrix(const int rows, const int cols) {
    int **matrix = calloc(rows, sizeof(int *));

    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    return matrix;
}

void fillMatrix(int **matrix, const int rows, const int cols) {
    if (matrix == NULL) {
        printf("Matrix is NULL\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 30;
        }
    }
}

void fillArray(int *array, const int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 1 + rand() % 30;
    }
}

void printMatrix(int **matrix, const int rows, const int cols) {
    if (matrix == NULL) {
        printf("Matrix is NULL\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void printArray(const int *array, const int size) {
    for (int i = 0; i < size; i++) {
        printf("%d\t", array[i]);
    }
}

void freeMatrix(int **matrix, const int rows) {
    if (matrix == NULL) {
        printf("Matrix is NULL\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }

    matrix = NULL;
    free(matrix);
}

double executionTime(const double start, const double end) {
    return end - start;
}

struct returnType rowExtractionSumAndMax(int **matrix, const int rows, const int cols, const int numThreads) {
    struct returnType out;
    out.maxValue = INT_MIN;

    int *array = arrayAllocation(numThreads);

    int i = 0, sum = 0;

    const double startSumRegionTime = omp_get_wtime();

# pragma omp parallel private(i) shared(array, rows, cols, matrix) num_threads(numThreads) reduction(+: sum)
    {
        for (i = 0; i < cols; i++) {
            sum += matrix[omp_get_thread_num()][i];
            array[omp_get_thread_num()] = sum;
        }
    }

    const double endSumRegionTime = omp_get_wtime();

    out.sumRegionTime = executionTime(startSumRegionTime, endSumRegionTime);

    printf("\nArray:\n");
    printArray(array, numThreads);

    printf("\n\nSum execution time: %f\n", out.sumRegionTime);

    const double startMaxRegionTime = omp_get_wtime();

    int maximum = INT_MIN;

#pragma omp parallel for private(i) shared(array) num_threads(numThreads) reduction(max: maximum)
    for (i = 0; i < numThreads; i++) {
        if (array[i] > maximum) {
            maximum = array[i];
        }
    }

    out.maxValue = maximum;

    const double endMaxRegionTime = omp_get_wtime();

    out.maxRegionTime = executionTime(startMaxRegionTime, endMaxRegionTime);

    return out;
}

int main(void) {
    srand(time(NULL));

    int numThreads, cols;

    printf("Insert matrix columns: ");
    scanf("%d", &cols);

    printf("Insert number of threads: ");
    scanf("%d", &numThreads);

    const int rows = numThreads;

    int **matrix = allocateMatrix(rows, cols);
    fillMatrix(matrix, rows, cols);
    printf("\nMatrix:\n");
    printMatrix(matrix, rows, cols);

    const struct returnType result = rowExtractionSumAndMax(matrix, rows, cols, numThreads);

    printf("\nMax value = %d\n", result.maxValue);

    printf("\nExecution time: %f\n", result.maxRegionTime);

    freeMatrix(matrix, rows);

    return 0;
}