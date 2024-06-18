/**
 * PROVA SCRITTA LABORATORIO TRACCIA 1: 29-06-2023
 *
 * Implementare un programma parallelo per l'ambiente multicore
 * con np unità processanti che impieghi la libreria OpenMP.
 *
 * Il programma deve essere organizzato come segue:
 *
 * 1. Ogni core deve generare (randomicamente) una matrice A di dimensione N/np x M,
 *    mentre solo il core master deve leggere (da tastiera) un vettore b di lunghezza M
 *
 * 2. Tutti i core devono calcolare localmente il prodotto c = A x b
 *
 * 3. Quindi, i core devono organizzarsi per determinare il massimo
 *    valore tra tutti gli elementi dei vettori locali c e il massimo
 *    globale tra tutti i vettori locali c
 *
 * 4. Infine, il core master deve stampare il valore massimo
 *    globale trovato ed il tempo d'esecuzione.
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <limits.h>

void fillMatrix(int **mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i][j] = 1 + rand() % 10;
        }
    }
}

int **initMatrix(int rows, int cols, int hasToFill) {
    int **mat = calloc(rows, sizeof(int *));
    for (int row = 0; row < rows; row++) {
        mat[row] = (int *) calloc(cols, sizeof(int));
    }

    if (hasToFill) {
        fillMatrix(mat, rows, cols);
    }

    return mat;
}

void fillArray(int *array, int size) {
    printf("\nInsert array elements\n");
    for (int i = 0; i < size; i++) {
        printf("array[%d]: ", i);
        scanf("%d", array + i);
    }
    printf("\n");
}

int *initArray(int size) {
    int *array = (int *) calloc(size, sizeof(int));
    fillArray(array, size);
    return array;
}

void printMatrix(int **mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("\t%d", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printArray(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("\t%d", array[i]);
    }
    printf("\n");
}

void matrixPerVectorWithLocalAndGlobalMax(int ***matrix, const int *array, const int rows, const int cols,
                                          const int numThreads) {
    int i = 0, j = 0, globalMax = INT_MIN;
    int *localMax = calloc(numThreads, sizeof(int));

    const double startFirstRegionTime = omp_get_wtime();

#pragma omp parallel private (i, j) shared(matrix, array, rows, cols, localMax) num_threads(numThreads)
    {
        const int threadId = omp_get_thread_num();
        const int matrixRows = rows / numThreads;
        matrix[threadId] = initMatrix(matrixRows, cols, 1);
        int **puntualProductPerThread = initMatrix(numThreads, matrixRows, 0);

#pragma omp critical
        {
            printf("Matrix for thread %d\n", threadId);
            printMatrix(matrix[threadId], rows / numThreads, cols);
        }

        for (i = 0; i < matrixRows; i++) {
            for (j = 0; j < cols; j++) {
                puntualProductPerThread[threadId][i] += matrix[threadId][i][j] * array[j];
            }
        }

#pragma omp critical
        {
            printf("Puntual product array for thread %d: ", threadId);
            printArray(puntualProductPerThread[threadId], matrixRows);
            printf("\n");
        }

        for (i = 0; i < matrixRows; i++) {
            if (puntualProductPerThread[threadId][i] > localMax[threadId]) {
                localMax[threadId] = puntualProductPerThread[threadId][i];
            }
        }

#pragma omp critical
        {
            printf("Local max for thread %d: %d\n", threadId, localMax[threadId]);

            for (i = 0; i < numThreads; i++) {
                free(puntualProductPerThread[i]);
            }
        }
    }

    const double endFirstRegionTime = omp_get_wtime();

    printf("Execution time for first region: %lf\n", endFirstRegionTime - startFirstRegionTime);

    const double startSecondRegionTime = omp_get_wtime();

#pragma omp parallel for private(i) shared(localMax, globalMax) num_threads(numThreads)
    for (i = 0; i < numThreads; i++) {
        if (localMax[i] > globalMax) {
            globalMax = localMax[i];
        }
    }

    const double endSecondRegionTime = omp_get_wtime();

    printf("Execution time for second region: %lf\n", endSecondRegionTime - startSecondRegionTime);

    printf("Global max: %d\n", globalMax);

    for (i = 0; i < numThreads; i++) {
        free(matrix[i]);
    }

    free(array);
    free(localMax);
}

int main() {
    int rows, cols, numThreads;

    srand(time(NULL));

    printf("Insert thread numbers: ");
    scanf("%d", &numThreads);

    printf("\nInsert matrix rows: ");
    scanf("%d", &rows);

    printf("\nInsert matrix cols: ");
    scanf("%d", &cols);

    int ***matrixPerThread = calloc(numThreads, sizeof(int **));
    const int *array = initArray(cols);

    matrixPerVectorWithLocalAndGlobalMax(matrixPerThread, array, rows, cols, numThreads);

    return 0;
}
