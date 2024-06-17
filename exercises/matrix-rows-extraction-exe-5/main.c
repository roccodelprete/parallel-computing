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

struct returnType {
    int **results;
    double time;
};

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

struct returnType extractRows(int **matrix, const int rows, const int cols, const int numThreads) {
    int i = 0, j = 0;

    struct returnType out;
    out.results = matrixAllocation(rows, cols);

    for (int k = 0; k < rows / numThreads; k++) {
        for (int l = 0; l < cols; l++) {
            out.results[k][l] = 1;
        }
    }

    const double startTime = omp_get_wtime();

#pragma omp parallel private(i, j) shared(matrix, out, rows, cols, numThreads) num_threads(numThreads)
    {
        const int threadID = omp_get_thread_num();
        const int rowsPerThread = rows / numThreads;
        const int rest = rows % numThreads;
        int start, end;

        if (threadID < rest) {
            start = threadID * (rowsPerThread + 1);
            end = start + (rowsPerThread + 1);
        } else {
            start = threadID * rowsPerThread + rest;
            end = start + rowsPerThread;
        }

        for (i = start; i < end; i++) {
            for (j = 0; j < cols; j++) {
                out.results[i][j] = matrix[i][j] * matrix[i + 1][j];
            }
        }
    }

    const double endTime = omp_get_wtime();

    out.time = getExecutionTime(startTime, endTime);

    return out;
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

    const struct returnType result = extractRows(matrix, rows, cols, numThreads);

    printf("\nResult: \n");
    printMatrix(result.results, rows / numThreads, cols);

    printf("\nExecution time: %f\n", result.time);

    freeMatrix(matrix, rows);

    return 0;
}
