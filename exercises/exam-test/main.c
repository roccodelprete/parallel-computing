#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
 * Genera una matrice B NxN e due vettori a e b, anch'essi di dimensione N, nel core master.
 * In parallelo costruire una nuova matrice A la cui diagonale è ottenuta sommando i valori della diagonale di B con i valori del vettore b.
 * Ottenuta la nuova matrice, eseguire un prodotto matrice-vettore con il vettore a, utilizzando la decomposizione per blocchi di colonne.
 * Stampa il risultato finale e il tempo di esecuzione.
 */

int** matrixAllocation(const int rows, const int columns) {
    int **matrix = calloc(rows, sizeof(int*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(columns, sizeof(int));
    }

    return matrix;
}

int *vectorAllocation(const int size) {
    int *vector = calloc(size, sizeof(int));

    return vector;
}

void freeMatrix(int **matrix, int const rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void freeVector(int *vector) {
    free(vector);
}

void fillMatrix(int **matrix, const int rows, const int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
    }
}

void fillVector(int *vector, const int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = 1 + rand() % 10;
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

void printVector(const int *vector, const int size) {
    for (int i = 0; i < size; i++) {
        printf("%d\t", vector[i]);
    }
    printf("\n");
}

void getExecutionTime(const double startTime, const double endTime) {
    const double time = endTime - startTime;
    printf("\nExecution time: %lf\n", time);
}

int **getDiagonal(int **matrix, const int rows, const int *vector) {
    int i = 0;

   const double startTime = omp_get_wtime();

    # pragma omp parallel for private(i) shared(matrix, vector, rows)
        for (i = 0; i < rows; i++) {
           matrix[i][i] += vector[i];
        }

    const double endTime = omp_get_wtime();

    getExecutionTime(startTime, endTime);

    return matrix;
}

int *matrixVectorMult(int **matrix, const int *vector, const int rows, const int cols) {
    int *result = vectorAllocation(rows);
    int i = 0, j = 0;
    int sum = 0;

    const double startTime = omp_get_wtime();

    #pragma omp parallel for private(i, j) shared(matrix, vector, result, rows, cols) reduction(+:sum)
    for (i = 0; i < rows; i++) {
        result[i] = 0;
        for (j = 0; j < cols; j++) {
            sum += matrix[j][i] * vector[i];
        }
        result[i] = sum;
        sum = 0;
    }

    const double endTime = omp_get_wtime();

    getExecutionTime(startTime, endTime);

    return result;
}

int main(void) {
    srand(time(NULL));

    int rowsAndColumns;

    printf("Insert rows and columns number: ");
    scanf("%d", &rowsAndColumns);

    int **firstMatrix = matrixAllocation(rowsAndColumns, rowsAndColumns);
    fillMatrix(firstMatrix, rowsAndColumns, rowsAndColumns);
    printf("Matrix\n");
    printMatrix(firstMatrix, rowsAndColumns, rowsAndColumns);

   int *vector = vectorAllocation(rowsAndColumns);
    fillVector(vector, rowsAndColumns);
    printf("\nVector\n");
    printVector(vector, rowsAndColumns);

    firstMatrix = getDiagonal(firstMatrix, rowsAndColumns, vector);
    printf("\nDiagonal\n");
    printMatrix(firstMatrix, rowsAndColumns, rowsAndColumns);

    int *vectorA = vectorAllocation(rowsAndColumns);
    fillVector(vectorA, rowsAndColumns);
    printf("\nVector A\n");
    printVector(vectorA, rowsAndColumns);

    const int *resultVector = matrixVectorMult(firstMatrix, vectorA, rowsAndColumns, rowsAndColumns);
    printf("\nResult vector\n");
    printVector(resultVector, rowsAndColumns);

    freeVector(vector);
    freeVector(vectorA);
    freeMatrix(firstMatrix, rowsAndColumns);

    return 0;
}
