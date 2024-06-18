#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

/**
* Implementare un programma parallelo per l’ambiente
* multicore con np unità processanti che impieghi la
* libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve generare
* una matrice A di dimensione N×M. Quindi, ogni core
* deve estrarre N/p colonne ed effettuare localmente la
* somma degli elementi delle sottomatrici estratte,
* conservando il risultato in un vettore b di dimensione
* M.
*/

void fillMatrix(int **mat, int rows, int cols) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            mat[i][j] = 1 + rand() % 10;
        }
    }
}

int **initMatrix(int rows, int cols) {
    int **mat = calloc(rows, sizeof(int *));
    for (int row=0; row<rows; row++) {
        mat[row] = (int *) calloc(cols, sizeof(int));
    }

    fillMatrix(mat, rows, cols);

    return mat;
}

void printMatrix(int **mat, int rows, int cols) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            printf("\t%d", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printArray(int *arr, int size) {
    for (int i=0; i<size; i++) {
        printf("\t%d", *(arr+i));
    }
    printf("\n");
}

void matrixColumnsExtractionWithSum(int **matrix, const int rows, const int cols, int numThreads) {
    int *result = calloc(cols, sizeof(int));
    int i, j;

    const double start = omp_get_wtime();
#pragma omp parallel private(i,j) shared(matrix, result, rows, cols) num_threads(numThreads)
    {
        int threadId = omp_get_thread_num();
        int chunk_size = cols / numThreads;
        int rest = cols % numThreads;
        int start_col, end_col;

        if (threadId < rest) {
            start_col = threadId * (chunk_size + 1);
            end_col = start_col + chunk_size + 1;
        } else {
            start_col = threadId * chunk_size + rest;
            end_col = start_col + chunk_size;
        }

        for (j = start_col; j < end_col; j++) {
            for (i = 0; i < rows; i++) {
                result[j] += matrix[i][j];
            }
        }
    }

    const double end = omp_get_wtime();

    printf("\nExecution time = %lf\n", end - start);

    printf("\nResult array:\n");
    printArray(result, cols);

    for (int k = 0; k < rows; k++) {
        free(matrix[k]);
    }
    free(matrix);
    free(result);
}

int main() {
    int rows, cols, numThreads;

    srand(time(NULL));

    printf("Insert thread numbers: ");
    scanf("%d", &numThreads);

    printf("\nInsert matrix rows: ");
    scanf("%d", &rows);

    printf("\nInsert matrix cols: ");
    scanf("%d", &cols);

    int **matrix = initMatrix(rows, cols);

    printf("Matrix:\n");
    printMatrix(matrix, rows, cols);

    matrixColumnsExtractionWithSum(matrix, rows, cols, numThreads);

    return 0;
}