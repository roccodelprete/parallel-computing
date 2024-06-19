#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <limits.h>

/**
* Implementare un programma parallelo per
* l’ambiente multicore con np unità processanti che
* impieghi la libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master generare una
* matrice A di dimensione M×M. Quindi, i core devo
* collaborare valutare il minimo valore tra gli elementi di
* A e successivamente costruire la matrice B i cui
* elementi sono dati dalla somma di tale valore minimo
* per gli elementi di posto corrispondente della matrice
* di partenza.
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

double getExecutionTime(const double start, const double end) {
    return end - start;
}

void matrixMinimumSum(int **matrix, const int rows, const int cols, int threads){
    int min = INT_MAX;
    int i, j;

    const double startFirstRegionTime = omp_get_wtime();

#pragma omp parallel for shared(matrix, rows, cols) private(i, j) reduction(min:min) num_threads(threads)
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols ; j++){
            if(matrix[i][j] < min)
                min = matrix[i][j];
        }
    }

    const double endFirstRegionTime = omp_get_wtime();

    printf("\nMinimum value: %d\n", min);

    printf("\nFirst region execution time: %lf\n", getExecutionTime(startFirstRegionTime, endFirstRegionTime));

    const double startSecondRegion = omp_get_wtime();

#pragma omp parallel for shared(matrix, min, rows, cols) private(i, j) num_threads(threads)
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols ; j++){
            matrix[i][j] += min;
        }
    }

    const double endSecondRegion = omp_get_wtime();

    printf("\nMatrix after sum:\n");
    printMatrix(matrix, rows, cols);

    printf("\nSecond region execution time: %lf\n", getExecutionTime(startSecondRegion, endSecondRegion));

    freeMatrix(matrix, rows);
}


int main(void) {

    int cols, threads;
    srand(time(NULL));

    printf("Insert threads number: ");
    scanf("%d", &threads);
    printf("\n");

    printf("Insert matrix columns: ");
    scanf("%d", &cols);
    printf("\n");
    const int rows = cols;

    int **matrix = matrixAllocation(cols, cols, 1);
    printf("Matrix:\n");
    printMatrix(matrix, rows, cols);

    matrixMinimumSum(matrix, rows, cols, threads);

    return 0;
}
