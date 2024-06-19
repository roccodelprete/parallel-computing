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

void fillMatrix(int **matrix, const int rows, const int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
    }
}

int **matrixAllocation(const int rows, const int cols, const int hasToFill) {
    int **matrix = calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    if (hasToFill) {
        fillMatrix(matrix, rows, cols);
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

void extractRowsAndPuntualProductCalculation(int **matrix, const int rows, const int cols, const int numThreads) {
    int i = 0, j = 0;

    const double startTime = omp_get_wtime();
    int **results = matrixAllocation(numThreads, cols, 0);

     for (i = 0; i < numThreads; i++) {
         for (j = 0; j < cols; j++) {
             results[i][j] = 1;
         }
     }

#pragma omp parallel private(i, j) shared(matrix, results, cols, numThreads) num_threads(numThreads)
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

        if (end > rows) {
            end = rows;
        }

        for (i = start; i < end; i++) {
            for (j = 0; j < cols; j++) {
                results[threadID][j] *= matrix[i][j];
            }
        }

#pragma omp critical
        {
            printf("\nThread %d – Puntual product from row %d to %d: \t", threadID, start, end - 1);
            printArray(results[threadID], cols);
        }
    }

    const double endTime = omp_get_wtime();

    const double time = getExecutionTime(startTime, endTime);

    printf("\nExecution time: %f\n", time);

    freeMatrix(matrix, rows);
    freeMatrix(results, numThreads);
}

int main(void) {
    srand(time(NULL));

    int rows;
    printf("Insert rows number: ");
    scanf("%d", &rows);
    const int cols = rows;

    int **matrix = matrixAllocation(rows, cols, 1);
    printf("\nMatrix: \n");
    printMatrix(matrix, rows, cols);

    int numThreads;
    printf("\nInsert number of threads: ");
    scanf("%d", &numThreads);

    extractRowsAndPuntualProductCalculation(matrix, rows, cols, numThreads);

    return 0;
}
