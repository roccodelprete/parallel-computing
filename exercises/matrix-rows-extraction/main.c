#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
* Implementare un programma parallelo per l’ambiente
* multicore con np unità processanti che impieghi la
* libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve generare
* una matrice di dimensione N×N, ogni core deve
* estrarre N/np righe e calcolare il prodotto puntuale tra
* i vettori corrispondenti alle righe estratte.
 */

int** matrixAllocation(const int rows, const int cols) {
    int **matrix = calloc(rows, sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    return matrix;
}

int *arrayAllocation(const int size) {
    int *vector = calloc(size, sizeof(int));

    return vector;
}

void freeMatrix(int **matrix, const int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void fillMatrix(int **matrix, const int rows, const int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
    }
}

void printMatrix(int **matrix, const int rows, const int cols) {
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
    printf("\n");
}

double getExecutionTime(const double start, const double end) {
    return end - start;
}

void extractRows(int **matrix, const int rows, const int cols, const int numThreads) {
#pragma omp parallel shared(matrix, rows, cols, numThreads) num_threads(numThreads)
    {
        const int threadID = omp_get_thread_num();
        const int rowsPerThread = rows / numThreads;
        const int startRow = threadID * rowsPerThread;
        int endRow = startRow + rowsPerThread;

        if (threadID == numThreads - 1) {
            endRow += rows % numThreads;
        }

        for (int i = startRow; i < endRow; i += 2) {
            const int *row1 = matrix[i];
            const int *row2 = matrix[i + 1];
            int *result = calloc(cols, sizeof(int));

            for (int j = 0; j < cols; j++) {
                result[j] = row1[j] * row2[j];
            }

            #pragma omp critical
            {
                printf("\nThread %d, Rows %d and %d product: \n", threadID, i, i + 1);
                printArray(result, cols);
            }
            free(result);
        }
    }
}

int main(void) {
    int rows;
    printf("Insert rows number: ");
    scanf("%d", &rows);
    const int cols = rows;

    int **matrix = matrixAllocation(rows, cols);
    fillMatrix(matrix, rows, cols);
    printf("\nMatrix: \n");
    printMatrix(matrix, rows, cols);

    int numThreads;
    printf("\nInsert number of threads: ");
    scanf("%d", &numThreads);

    extractRows(matrix, rows, cols, numThreads);

    freeMatrix(matrix, rows);

    return 0;
}
