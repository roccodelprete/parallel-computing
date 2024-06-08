#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MATRIX_COLS 4
#define MATRIX_ROWS 4

int **allocateMatrix(int rows, int cols) {
    int **matrix = calloc(rows, sizeof(int *));

    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    return matrix;
}

void fillMatrix(int **matrix, const int rows, const int cols) {
    srand(time(NULL));

    if (matrix == NULL) {
        printf("Matrix is NULL\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
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

int main(void) {
    int choice;

    int **firstMatrix = allocateMatrix(MATRIX_ROWS, MATRIX_COLS);
    int **secondMatrix = allocateMatrix(MATRIX_ROWS, MATRIX_COLS);

    int alpha;

    fillMatrix(secondMatrix, MATRIX_ROWS, MATRIX_COLS);
    printf("MATRIX\n");
    printMatrix(secondMatrix, MATRIX_ROWS, MATRIX_COLS);

    printf("\nInsert alpha: ");
    scanf("%d", &alpha);

    printf("\nInsert your choice (1, 2): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
#pragma omp parallel for shared(firstMatrix, secondMatrix, alpha)
            for (int i = 0; i < MATRIX_ROWS; i++) {
                for (int j = 0; j < MATRIX_COLS; j++) {
                    firstMatrix[i][j] = alpha * secondMatrix[i][j];
                }
            }

            printf("\nRESULT MATRIX\n");
            printMatrix(firstMatrix, MATRIX_ROWS, MATRIX_COLS);
            break;
        case 2:
# pragma omp parallel for shared(firstMatrix, secondMatrix)
            for (int i = 0; i < MATRIX_COLS; i++) {
                for (int j = 0; j < MATRIX_ROWS; j++) {
                    firstMatrix[i][j] = alpha * secondMatrix[i][j];
                }
            }

            printf("\nRESULT MATRIX\n");
            printMatrix(firstMatrix, MATRIX_ROWS, MATRIX_COLS);
            break;
        default:
            printf("Invalid choice\n");
            break;
    }

    freeMatrix(firstMatrix, MATRIX_ROWS);
    freeMatrix(secondMatrix, MATRIX_ROWS);

    return 0;
}
