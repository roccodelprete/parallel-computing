#include <math.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
* Implementare un programma parallelo per
* l’ambiente multicore con np×np unità processanti che
* impieghi la libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve generare
* una matrice A di dimensione N×N. Quindi, ogni core
* deve sommare tra loro gli elementi di un blocco
* quadrato della matrice A di dimensione (N/np)×(N/np),
* conservando i valori in un vettore b.
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
            matrix[i][j] = 1 + rand() % 30;
        }
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

    matrix = NULL;
    free(matrix);
}

double executionTime(const double start, const double end) {
    return end - start;
}

struct returnType matrixBlockSum(int **matrix, const int rows, const int cols, const int numThreads) {
    struct returnType out;
    out.array = arrayAllocation(numThreads * numThreads);

    int i = 0, j = 0;

    const double startTime = omp_get_wtime();
# pragma omp parallel private(i, j) shared(out, rows, cols, matrix) num_threads(numThreads * numThreads)
    {
        const int threadId = omp_get_thread_num();
        const int startRow = threadId / numThreads;
        const int startCol = threadId % numThreads;

        for (i = startRow * rows / numThreads; i < (startRow + 1) * rows / numThreads; i++) {
            for (j = startCol * cols / numThreads; j < (startCol + 1) * cols / numThreads; j++) {
                out.array[threadId] += matrix[i][j];
            }
        }
    }

    const double endTime = omp_get_wtime();

    out.time = executionTime(startTime, endTime);

    return out;
}

int main(void) {
    srand(time(NULL));

    int numThreads, rows;

    printf("Insert matrix rows: ");
    scanf("%d", &rows);
    const int cols = rows;

    printf("\nInsert number of threads: ");
    scanf("%d", &numThreads);

    int **matrix = allocateMatrix(rows, cols);
    fillMatrix(matrix, rows, cols);
    printf("\nMatrix:\n");
    printMatrix(matrix, rows, cols);

    const struct returnType result = matrixBlockSum(matrix, rows, cols, numThreads);

    printf("\nResult matrix:\n");
    printArray(result.array, numThreads * numThreads);

    printf("\n\nExecution time: %f\n", result.time);

    freeMatrix(matrix, rows);
    free(result.array);

    return 0;
}

