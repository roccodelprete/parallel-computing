#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define NUM_THREADS 4
#define ITERATIONS 10
#define CHUNK 2

int main(void) {
    /**
     * Schedule types:
     * - static: each thread gets a chunk of iterations
     * - dynamic: each thread gets a chunk of iterations, but when it finishes it gets another chunk
     * - guided: each thread gets a chunk of iterations, but the chunk size decreases over time
     */
    #pragma omp parallel for num_threads(NUM_THREADS) schedule(guided)
        for (int i = 0; i < ITERATIONS; i++) {
            sleep(i);

            printf("Iteraction %d from thread %d\n", i, omp_get_thread_num());
        }

    printf("\nAll threads finished\n");

    return 0;
}
