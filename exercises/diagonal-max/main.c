#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define ROWS 4
#define COLS 4
#define THREADS 4

int **matrixAllocation() {
    int **matrix = calloc(ROWS, sizeof(int *));
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = calloc(COLS, sizeof(int));
    }

    return matrix;
}

int *arrayAllocation() {
    int *vector = calloc(ROWS, sizeof(int));

    return vector;
}

void freeMatrix(int **matrix) {
    for (int i = 0; i < ROWS; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void freeArray(int *vector) {
    free(vector);
}

void fillMatrix(int **matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
    }
}

void printMatrix(int **matrix, char *matrixTitle) {
    printf("%s", matrixTitle);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void printArray(const int *vector, char *arrayTitle) {
    printf("%s", arrayTitle);

    for (int i = 0; i < ROWS; i++) {
        printf("%d\t", vector[i]);
    }
    printf("\n");
}

void getExecutionTime(const double start, const double end, char *parallelRegionTitle) {
    printf("\nExecution time for %s = %f\n", parallelRegionTitle, end - start);
}

int getMax(const int *vector) {
    int maxValue = vector[0];

    const double startTime = omp_get_wtime();

    #pragma omp parallel for shared(vector) reduction(max: maxValue) num_threads(THREADS)
    for (int i = 1; i < ROWS; i++) {
        if (vector[i] > maxValue) {
            maxValue = vector[i];
        }
    }

    const double endTime = omp_get_wtime();

    getExecutionTime(startTime, endTime, "max calculation");

    return maxValue;
}

int diagonalMax(int **matrix) {
    int *diagonal = arrayAllocation();
    int maxValue = 0;

    const double startTime = omp_get_wtime();

    #pragma omp parallel for shared(matrix, diagonal) reduction(max: maxValue) num_threads(THREADS)
    for (int i = 0; i < ROWS; i++) {
        diagonal[i] = matrix[i][i];
    }

    const double endTime = omp_get_wtime();

    getExecutionTime(startTime, endTime, "diagonal extraction");

    maxValue = getMax(diagonal);

    printArray(diagonal, "\nDiagonal\n");

    freeArray(diagonal);

    return maxValue;
}

int main(void) {
    srand(time(NULL));

    int **matrix = matrixAllocation();
    fillMatrix(matrix);
    printMatrix(matrix, "Matrix\n");

    const int max = diagonalMax(matrix);
    printf("\nMax = %d\n", max);

    freeMatrix(matrix);

    return 0;
}
