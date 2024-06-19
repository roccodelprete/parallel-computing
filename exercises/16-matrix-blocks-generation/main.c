#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
 * Implementare  un  programma  parallelo  per 
 * l’ambiente multicore con nq×np unità processanti che 
 * impieghi la libreria OpenMP. Il programma deve essere 
 * organizzato come segue: il core master deve costruire 
 * una matrice A, di dimensione M×N, i cui blocchi sono 
 * generati casualmente e in parallelo da ogni core.
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

void fillMatrix(int **matrix, const int rows, const int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
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

double getExecutionTime(const double startTime, const double endTime) {
    return endTime - startTime;
}

void matrixBlocksGeneration(int **matrix, const int rows, const int cols, const int numThreads) {
    int i, j;

    const double startTime = omp_get_wtime();

    #pragma omp parallel for private(i, j) shared(matrix, rows, cols) num_threads(numThreads)
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            matrix[i][j] = 1 + rand() % 10;
        }
    }

    const double endTime = omp_get_wtime();

    printf("Matrix: \n");
    printMatrix(matrix, rows, cols);

    const double exeTime = getExecutionTime(startTime, endTime);

    printf("\nExecution time = %lf\n", exeTime);

    freeMatrix(matrix, rows);
}

int main(void) {
    srand(time(NULL));

    int rows, cols, alpha, numThreads;

    printf("Insert matrix rows: ");
    scanf("%d", &rows);
    printf("\n");
    
    printf("Insert matrix columns: ");
    scanf("%d", &cols);
    printf("\n");

    int **matrix = matrixAllocation(cols, rows);
    
    printf("Insert number of threads: ");
    scanf("%d", &numThreads);
    printf("\n");

    matrixBlocksGeneration(matrix, rows, cols, numThreads);

    return 0;
}
