#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10

void fillArray(int *array, int size) {
    srand(time(NULL));

    for (int i = 0; i < size; i++) {
        array[i] = 1 + rand() % 10;
    }
}

void printArray(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d\t", array[i]);
    }

    printf("\n");
}

int main(void) {
    int *firstArray = calloc(SIZE, sizeof(int));
    int *secondArray = calloc(SIZE, sizeof(int));
    int result = 0;

    fillArray(firstArray, SIZE);
    fillArray(secondArray, SIZE);

    printf("FIRST ARRAY\n");
    printArray(firstArray, SIZE);

    printf("SECOND ARRAY\n");
    printArray(secondArray, SIZE);

    #pragma omp parallel for shared(firstArray, secondArray) reduction(+:result)
        for (int i = 0; i < SIZE; i++) {
            result += firstArray[i] * secondArray[i];
        }

    printf("RESULT = %d\n", result);

    return 0;
}
