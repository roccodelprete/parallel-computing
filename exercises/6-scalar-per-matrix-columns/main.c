#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
* Implementare un programma parallelo per l’ambiente
* multicore con np unità processanti che impieghi la
* libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve leggere
* una matrice A di dimensione N×N ed uno scalare alpha.
* Quindi, i core devono collaborare per calcolare il
* prodotto tra lo scalare alpha e la matrice A, sfruttando
* una strategia di parallelizzazione che usi la
* distribuzione per colonne della matrice A come
* decomposizione del dominio.
*/

struct ReturnType {
    int **matrix;
    double time;
};

int **allocateMatrix(const int rows, const int cols) {
    int **matrix = calloc(rows, sizeof(int *));

    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    return matrix;
}

void fillMatrix(int **matrix, const int rows, const int cols) {
    printf("\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("Insert matrix[%d][%d]: ", i, j);
            scanf("%d", &matrix[i][j]);
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

double executionTime(const double start, const double end) {
    return end - start;
}

struct ReturnType scalarPerMatrixProduct(int **matrix, const int alpha, const int rows, const int cols) {
    struct ReturnType out;
    out.matrix = allocateMatrix(rows, cols);

    int i = 0, j = 0;

    const double start = omp_get_wtime();

# pragma omp parallel for private(i, j) shared(matrix, out, alpha, rows, cols)
    for (i = 0; i < cols; i++) {
        for (j = 0; j < rows; j++) {
            out.matrix[i][j] = alpha * matrix[i][j];
        }
    }

    const double end = omp_get_wtime();

    out.time = executionTime(start, end);

    return out;
}

int main(void) {
    int alpha, rows;

    printf("Insert matrix rows: ");
    scanf("%d", &rows);
    const int cols = rows;

    int **matrix = allocateMatrix(rows, cols);
    fillMatrix(matrix, rows, cols);

    printf("\nInsert alpha: ");
    scanf("%d", &alpha);

    const struct ReturnType result = scalarPerMatrixProduct(matrix, alpha, rows, cols);

    printf("\nResult Matrix\n");
    printMatrix(result.matrix, rows, cols);

    printf("\nExecution time: %f\n", result.time);

    freeMatrix(matrix, rows);
    freeMatrix(result.matrix, rows);

    return 0;
}
