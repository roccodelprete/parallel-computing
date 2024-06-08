#include <stdio.h>
#include <omp.h>

int main(void) {
    int idThread, numThreadsPlaying;

    #pragma omp parallel private(idThread), shared(numThreadsPlaying)
    {
        idThread = omp_get_thread_num();
        numThreadsPlaying = omp_get_num_threads();

        printf("Hello from thread %d, nthreads %d\n", idThread, numThreadsPlaying);
    }

    return 0;
}
