#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 3
#define COLS 3

int** matrixAllocation() {
    int **matrix = calloc(ROWS, sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = calloc(COLS, sizeof(int));
    }

    return matrix;
}

int *vectorAllocation() {
    int *vector = calloc(ROWS, sizeof(int));

    return vector;
}

void freeMatrix(int **matrix) {
    for (int i = 0; i < ROWS; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void freeVector(int *vector) {
    free(vector);
}

void fillMatrix(int **matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
    }
}

void fillVector(int *vector) {
    for (int i = 0; i < ROWS; i++) {
        vector[i] = 1 + rand() % 10;
    }
}

void printMatrix(int **matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void printVector(const int *vector) {
    for (int i = 0; i < ROWS; i++) {
        printf("%d\t", vector[i]);
    }
    printf("\n");
}

int *matrixVectorMult(int **matrix, const int *vector) {
    int *result = vectorAllocation();

    #pragma omp parallel for shared(matrix, vector, result)
    for (int i = 0; i < ROWS; i++) {
        result[i] = 0;
        for (int j = 0; j< COLS; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }

    return result;
}

int main(void) {
    srand(time(NULL));

    int **firstMatrix = matrixAllocation();
    fillMatrix(firstMatrix);
    printf("Matrix\n");
    printMatrix(firstMatrix);

   int *vector = vectorAllocation();
    fillVector(vector);
    printf("\nVector\n");
    printVector(vector);

    const int *result = matrixVectorMult(firstMatrix, vector);
    printf("\nResult\n");
    printVector(result);

    freeMatrix(firstMatrix);
    freeVector(vector);
    freeVector(result);

    return 0;
}
