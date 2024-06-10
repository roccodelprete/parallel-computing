#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
* Implementare un programma parallelo per l’ambiente
* multicore con np unità processanti che impieghi la
* libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve generare
* due vettori casuali di dimensione N. Ogni core deve
* sommare ad ogni elemento dei due vettori il proprio
* identificativo e collaborare alla costruzione di una
* matrice, di dimensione N×M, con M=2np, le cui
* colonne, a due a due, sono i due vettori ottenuti nella
* fase precedente.
*/


struct returnType {
    int **matrix;
    double time;
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

struct returnType matrixColumnsGeneration(const int *firstArray, const int *secondArray, const int size, const int numThreads) {
    const int matrixRows = size;
    const int matrixCols = 2 * numThreads;

    struct returnType out;
    out.matrix = allocateMatrix(matrixRows, matrixCols);
    fillMatrix(out.matrix, matrixRows, matrixCols);

    int i = 0, j = 0;

    const double start = omp_get_wtime();
# pragma omp parallel for private(i, j) shared(out, size, firstArray, secondArray)
    for (j = 0; j < matrixCols; j += 2) {
        for (i = 0; i < matrixRows; i++) {
            const int threadId = omp_get_thread_num();
            out.matrix[i][j] = firstArray[i] + threadId;
            out.matrix[i][j + 1] = secondArray[i] + threadId;
        }
    }

    const double end = omp_get_wtime();

    out.time = executionTime(start, end);

    return out;
}

int main(void) {
    srand(time(NULL));

    int size, numThreads;

    printf("Insert arrays size: ");
    scanf("%d", &size);

    printf("\nInsert number of threads: ");
    scanf("%d", &numThreads);

    int *firstArray = arrayAllocation(size);
    fillArray(firstArray, size);

    int *secondArray = arrayAllocation(size);
    fillArray(secondArray, size);

    printf("\nFirst array:\n");
    printArray(firstArray, size);

    printf("\n\nSecond array:\n");
    printArray(secondArray, size);

    const struct returnType result = matrixColumnsGeneration(firstArray, secondArray, size, numThreads);

    printf("\n\nResult matrix:\n");
    printMatrix(result.matrix, size, 2 * numThreads);

    printf("\nExecution time: %f\n", result.time);

    free(firstArray);
    free(secondArray);
    freeMatrix(result.matrix, size);

    return 0;
}
