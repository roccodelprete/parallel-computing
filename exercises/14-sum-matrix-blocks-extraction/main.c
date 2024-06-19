#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

/**
* Implementare un programma parallelo per
* l’ambiente multicore con np×np unità processanti che
* impieghi la libreria OpenMP. Il programma deve essere
* organizzato come segue: il core master deve leggere
* una matrice A di dimensione M×M. Quindi, ogni core
* deve estrarre un blocco della matrice A di dimensione
* (M/np)×(M/np), conservando i valori in np×np matrici.
* Infine, i core devono collaborare per calcolare la
* somma di tali matrici.
*/

/**
* Function to fill a matrix
* @param matrix: matrix to fill
* @param rows: number of rows
* @param cols: number of columns
* @param fillType: type of fill. Possible values: 0 (random), 1 (manual)
*/
void fillMatrix(int **matrix, const int rows, const int cols, int fillType) {
    if (fillType) {
        printf("\nInsert matrix values: \n");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("matrix[%d][%d]: ", i, j);
                scanf("%d", &matrix[i][j]);
            }
        }
        printf("\n");
    } else {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                matrix[i][j] = 1 + rand() % 10;
            }
        }
    }
}

/**
* Function to allocate a matrix
* @param rows: number of rows
* @param cols: number of columns
* @param hasToFill: flag to fill the matrix
* @param fillType: type of fill. Possible values: 0 (random), 1 (manual)
*/
int **matrixAllocation(const int rows, const int cols, const int hasToFill, const int fillType) {
    int **matrix = calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));
    }

    if (hasToFill) {
        fillMatrix(matrix, rows, cols, fillType);
    }

    return matrix;
}

int *arrayAllocation(const int size) {
    int *vector = calloc(size, sizeof(int));

    return vector;
}

void freeMatrix(int **matrix, const int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void free3DMatrix(int ***matrix, const int rows, const int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(matrix[i][j]);
        }
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

void printArray(const int *array, const int size) {
    for (int i = 0; i < size; i++) {
        printf("%d\t", array[i]);
    }
    printf("\n");
}

double getExecutionTime(const double start, const double end) {
    return end - start;
}

void sumMatrixBlocksExtraction(int **matrix, const int rows, const int cols, const int numThreads) {
    int i = 0, j = 0, k = 0;

    const double startTime = omp_get_wtime();

    int ***blocksExtracted = calloc(numThreads * numThreads, sizeof(int **));
    for (i = 0; i < numThreads * numThreads; i++) {
        blocksExtracted[i] = matrixAllocation(rows / numThreads, cols / numThreads, 0, 0);
    }

    int **sums = matrixAllocation(rows / numThreads, cols / numThreads, 0, 0);

#pragma omp parallel private(i, j) shared(matrix, blocksExtracted, cols, numThreads) num_threads(numThreads * numThreads)
    {
        const int threadID = omp_get_thread_num();
        const int blocksPerThread = rows / numThreads;
        const int rowStart = threadID / numThreads * blocksPerThread;
        const int colStart = threadID % numThreads * numThreads;

        **blocksExtracted[threadID] = matrixAllocation(blocksPerThread, blocksPerThread, 0, 0);

        for (i = 0; i < blocksPerThread; i++) {
            for (j = 0; j < blocksPerThread; j++) {
                blocksExtracted[threadID][i][j] = matrix[rowStart + i][colStart + j];
            }
        }

#pragma omp critical
        {
            printf("\nThread %d – Extraction block (%d:%d, %d:%d):\n", threadID, rowStart, rowStart + blocksPerThread, colStart, colStart + blocksPerThread);
            printMatrix(blocksExtracted[threadID], blocksPerThread, blocksPerThread);
        }

#pragma omp for collapse(3)
        for (i = 0; i < numThreads * numThreads; i++) {
            for (j = 0; j < blocksPerThread; j++) {
                for (k = 0; k < blocksPerThread; k++) {
                    sums[j][k] += blocksExtracted[i][j][k];
                }
            }
        }
    }

    const double endTime = omp_get_wtime();

    const double time = getExecutionTime(startTime, endTime);

    printf("\nResulting matrix: \n");
    printMatrix(sums, cols / numThreads, cols / numThreads);

    printf("\nExecution time: %f\n", time);

    freeMatrix(matrix, rows);
    free3DMatrix(blocksExtracted, numThreads * numThreads, rows / numThreads);
    freeMatrix(sums, cols / numThreads);
}

int main(void) {
    srand(time(NULL));

    int cols;
    printf("Insert cols number: ");
    scanf("%d", &cols);
    const int rows = cols;

    int **matrix = matrixAllocation(rows, cols, 1, 1);
    printf("Matrix: \n");
    printMatrix(matrix, rows, cols);

    int numThreads;
    printf("\nInsert number of threads: ");
    scanf("%d", &numThreads);

    sumMatrixBlocksExtraction(matrix, rows, cols, numThreads);

    return 0;
}
