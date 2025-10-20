#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define N 10

int array[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Función que calculará la sumatoria parcial
void *calcular_sumatoria(void *arg) {
    int *thread_id = (int *)arg;
    int elements_per_thread = N / NUM_THREADS;
    int start = *thread_id * elements_per_thread;
    int end = start + elements_per_thread;
    int sum = 0;

    for (int i = start; i < end; i++) {
        sum += array[i];
    }

    int *result = (int *)malloc(sizeof(int));
    *result = sum;

    pthread_exit(result);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, calcular_sumatoria, &thread_ids[i])) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }

    int total_sum = 0;
    int *result;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], (void **)&result);
        total_sum += *result;
        free(result);
    }

    printf("Sumatoria final: %d\n", total_sum);

    return 0;
}

