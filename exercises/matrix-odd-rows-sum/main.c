#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
* Implementare un programma parallelo per
* l’ambiente multicore con np unità processanti che
* impieghi la libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve leggere
* una matrice di dimensione N×M, quindi i core devo
* collaborare per sommare in parallelo i vettori
* corrispondenti alle righe dispari della matrice.
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
    if (matrix == NULL) {
        printf("Matrix is NULL\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("matrix[%d][%d]: ", i, j);
            scanf("%d", &matrix[i][j]);
        }
        printf("\n");
    }
}

void fillArray(int *array, const int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 1 + rand() % 30;
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
}

double executionTime(const double start, const double end) {
    return end - start;
}

struct returnType matrixOddRowsSum(int **matrix, const int rows, const int cols, const int numThreads) {
    struct returnType out;
    out.array = arrayAllocation(cols);

    int i = 0, j = 0, sum = 0;

    const double start = omp_get_wtime();
# pragma omp parallel for private(i, j) shared(out, rows, cols, matrix) reduction(+:sum) num_threads(numThreads)
    for (j = 0; j < cols; j++) {
        for (i = 1; i < rows; i += 2) {
            sum += matrix[i][j];
        }
        out.array[j] = sum;
        sum = 0;
    }

    const double end = omp_get_wtime();

    out.time = executionTime(start, end);

    return out;
}

int main(void) {
    srand(time(NULL));

    int rows, cols, numThreads;

    printf("Insert matrix rows: ");
    scanf("%d", &rows);

    printf("\nInsert matrix cols: ");
    scanf("%d", &cols);

    printf("\nInsert number of threads: ");
    scanf("%d", &numThreads);
    printf("\n");

    int **matrix = allocateMatrix(rows, cols);
    fillMatrix(matrix, rows, cols);
    printf("Matrix:\n");
    printMatrix(matrix, rows, cols);
    printf("\n");

    const struct returnType result = matrixOddRowsSum(matrix, rows, cols, numThreads);

    printf("Result array:\n");
    printArray(result.array, cols);

    printf("\n\nExecution time: %f\n", result.time);

    freeMatrix(matrix, rows);
    free(result.array);

    return 0;
}
