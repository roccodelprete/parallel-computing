#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
* Implementare un programma parallelo per l’ambiente
* multicore con np unità processanti impieghi la libreria
* OpenMP. Il programma deve essere organizzato come
* segue: il core master deve leggere una matrice di
* dimensione N×N, quindi i core devo collaborare per
* ricopiare in parallelo gli elementi della diagonale
* principale in un vettore di lunghezza N. Infine, i core
* devono effettuare la somma degli elementi di tale
* vettore in parallelo.
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

void fillArray(int *array, const int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 1 + rand() % 10;
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

void matrixDiagonalSum(int **matrix, const int rows, const int cols, const int numThreads) {
    int *diagonal = calloc(rows, sizeof(int));
    int i = 0;

    const double startExtractionTime = omp_get_wtime();

#pragma omp parallel for private(i) shared(rows, diagonal, matrix) num_threads(numThreads)
    for (i = 0; i < rows; i++) {
        diagonal[i] = matrix[i][i];
    }

    const double endExtractionTime = omp_get_wtime();

    printf("\nDiagonal array \n");
    printArray(diagonal, rows);

    printf("\nExecution time diagonal extraction = %lf\n", getExecutionTime(startExtractionTime, endExtractionTime));

    int sum = 0;

    const double startSumTime = omp_get_wtime();

#pragma omp parallel for private(i) shared(diagonal, rows) num_threads(numThreads) reduction(+:sum)
    for (i = 0; i < rows; i++) {
        sum += diagonal[i];
    }

    const double endSumTime = omp_get_wtime();

    printf("\nSum = %d\n", sum);

    printf("\nExecution time sum = %lf\n", getExecutionTime(startSumTime, endSumTime));

    free(diagonal);
}

int main(void) {
    srand(time(NULL));

    int rows;
    printf("Enter the number of rows and cols: ");
    scanf("%d", &rows);
    const int cols = rows;

    int **matrix = matrixAllocation(rows, cols);
    fillMatrix(matrix, rows, cols);
    printf("\nMatrix \n");
    printMatrix(matrix, rows, cols);

    int numThreads;
    printf("\nEnter the number of threads: ");
    scanf("%d", &numThreads);

    matrixDiagonalSum(matrix, rows, cols, numThreads);

    freeMatrix(matrix, rows);

    return 0;
}
