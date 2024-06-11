#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int **initMatrix(int, int);
void printMatrix(int **, int, int);
void printArray(int *, int);

int main() {
    int rows, cols, p, i, j;
    int *b;
    int **A;
    double start, end;

    srand(time(NULL));

    printf("Inserire il numero di thread: ");
    scanf("%d", &p);

    printf("\nInserire il numero di righe della matrice: ");
    scanf("%d", &rows);

    printf("\nInserire il numero di colonne della matrice, che equivale alla dimensione del vettore risultante: ");
    scanf("%d", &cols);

    A = initMatrix(rows, cols);
    b = (int *) calloc(cols, sizeof(int));

    printf("Matrice A:\n");
    printMatrix(A, rows, cols);

    start = omp_get_wtime();

#pragma omp parallel private(i,j) shared(A, b, rows, cols) num_threads(p)
    {
        int tid = omp_get_thread_num(); //estraggo id thread corrente
        int chunk_size = cols / p; //calcolo quante colonne dare ad ogni core
        int r = cols % p; // calcolo il resto per capire quanti core avranno una colonna in piu
        int start_col, end_col;

        if (tid < r) { // tutti gli id con indice inferiore al resto
            start_col = tid * (chunk_size + 1); // ottengo la prima colonna da usare con la moltiplicazione tra l'indice dell'id e il numero di colonne + 1
            end_col = start_col + chunk_size + 1; // ottengo la colonna successiva alle colonne che effettivamente userò per il singolo core
        } else {
            start_col = tid * chunk_size + r; //start col in qureesto caso è uguale a id thread * numero di colonne + il resto
            end_col = start_col + chunk_size;
        }

        for (j = start_col; j < end_col; j++) {  //scorro le varie colonne
            for (i = 0; i < rows; i++) { // per ogni colonna sommo i vari valori
                b[j] += A[i][j];
            }
        }
    }

    end = omp_get_wtime();

    printf("\nTempo di esecuzione della regione parallela: %lf\n", end-start);

    printf("\nVettore risultante:\n");
    printArray(b, cols);

    for (int k=0; k<rows; k++) {
        free(A[k]);
    }
    free(A);
    free(b);

    return 0;
}

int **initMatrix(int rows, int cols) {
    int **mat = (int **) calloc(rows, sizeof(int *));
    for (int row=0; row<rows; row++) {
        mat[row] = (int *) calloc(cols, sizeof(int));
    }

    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            mat[i][j] = rand()%30;
        }
    }
    printf("\n");

    return mat;
}

void printMatrix(int **mat, int rows, int cols) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            printf("\t%d", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printArray(int *arr, int size) {
    for (int i=0; i<size; i++) {
        printf("\t%d", *(arr+i));
    }
    printf("\n");
}