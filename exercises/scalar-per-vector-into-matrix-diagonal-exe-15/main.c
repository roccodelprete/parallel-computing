#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
 * Implementare  un  programma  parallelo  per 
 * l’ambiente  multicore  con  np  unità  processanti  che 
 * impieghi la libreria OpenMP. Il programma deve essere 
 * organizzato come segue: il core master deve leggere un 
 * vettore a, di dimensione M, e uno scalare alpha. Quindi 
 * i core devono collaborare per costruire una matrice A, 
 * di dimensione M×M, la cui diagonale principale ha per 
 * elementi quelli del vettore a moltiplicati per lo scalare 
 * alpha.
 */

int** matrixAllocation(const int rows, const int columns) {
    int **matrix = calloc(rows, sizeof(int*));
    
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(columns, sizeof(int));
    }

    return matrix;
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

int* vectorInit(const int size) {
    int *vector = calloc(size, sizeof(int));

    for (int i = 0; i < size; i++) {
        printf("vector[%d]: ", i);
        scanf("%d", vector + i);
    }

    return vector;
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

double getExecutionTime(const double startTime, const double endTime) {
    return endTime - startTime;
}

void matrixBlocksGeneration(const int *vector, const int size, const int alpha, const int numThreads) {
    int **matrix = matrixAllocation(size, size);

    int i, j;

    const double startTime = omp_get_wtime();

    #pragma omp parallel for private(i, j) shared(vector, matrix, size) num_threads(numThreads)
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (i == j) {
                matrix[i][j] = alpha * vector[i];
            } else {
                matrix[i][j] = 1 + rand() % 10;
            }
        }
    }

    const double endTime = omp_get_wtime();

    printf("Matrix: \n");
    printMatrix(matrix, size, size);

    const double exeTime = getExecutionTime(startTime, endTime);

    printf("\nExecution time = %lf\n", exeTime);

    freeMatrix(matrix, size);
}

int main(void) {
    srand(time(NULL));

    int size, alpha, numThreads;

    printf("Insert array size: ");
    scanf("%d", &size);

    int *array = vectorInit(size);
    printf("\nArray: \n");
    printVector(array, size);

    printf("\nInsert alpha: ");
    scanf("%d", &alpha);
    printf("\n");
    
    printf("Insert number of threads: ");
    scanf("%d", &numThreads);
    printf("\n");

    matrixBlocksGeneration(array, size, alpha, numThreads);

    freeVector(array);

    return 0;
}
