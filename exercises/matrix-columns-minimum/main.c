#include <limits.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
* Implementare un programma parallelo per l’ambiente
* multicore con np unità processanti che impieghi la
* libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve generare
* una matrice A di dimensione N×M. Quindi, i core
* devono collaborare per calcolare il minimo tra gli
* elementi delle colonne di propria competenza e
* conservare tale valore in un vettore c di dimensione M.
*/

struct returnType {
    int *array;
    double time;
};

int *arrayAllocation(const int size) {
    int *vector = calloc(size, sizeof(int));

    return vector;
}

int **allocateMatrix(const int rows, const int cols) {
    int **matrix = calloc(rows, sizeof(int *));

    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    return matrix;
}

void fillMatrix(int **matrix, const int rows, const int cols) {
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

void printArray(const int *array, const int size) {
    for (int i = 0; i < size; i++) {
        printf("%d\t", array[i]);
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

struct returnType matrixMinColumns(int **matrix, const int rows, const int cols) {
    struct returnType out;
    out.array = arrayAllocation(cols);

    int i = 0, j = 0, minValue = INT_MAX;

    const double start = omp_get_wtime();
# pragma omp parallel for private(i, j) shared(matrix, out, rows, cols) reduction(min: minValue)
    for (i = 0; i < cols; i++) {
        for (j = 0; j < rows; j++) {
            if (matrix[j][i] < minValue) {
                minValue = matrix[j][i];
            }
        }

        out.array[i] = minValue;
    }

    const double end = omp_get_wtime();

    out.time = executionTime(start, end);

    return out;
}

int main(void) {
    srand(time(NULL));

    int rows, cols;

    printf("Insert matrix rows: ");
    scanf("%d", &rows);

    printf("\nInsert matrix columns: ");
    scanf("%d", &cols);

    int **matrix = allocateMatrix(rows, cols);
    fillMatrix(matrix, rows, cols);
    printf("\nMatrix\n");
    printMatrix(matrix, rows, cols);

    const struct returnType result = matrixMinColumns(matrix, rows, cols);

    printf("\nResult Array\n");
    printArray(result.array, cols);

    printf("\n\nExecution time: %f\n", result.time);

    freeMatrix(matrix, rows);
    free(result.array);

    return 0;
}
