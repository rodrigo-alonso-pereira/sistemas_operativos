#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define N 10

int array[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Estructura para pasar datos a la hebra
struct ThreadData {
    int start;
    int end;
    int result;
};

// Función que calculará la sumatoria parcial
void *calcular_sumatoria(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    int sum = 0;

    for (int i = data->start; i < data->end; i++) {
        sum += array[i];
    }

    data->result = sum;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        int elements_per_thread = N / NUM_THREADS;
        thread_data[i].start = i * elements_per_thread;
        thread_data[i].end = thread_data[i].start + elements_per_thread;

        if (pthread_create(&threads[i], NULL, calcular_sumatoria, &thread_data[i])) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }

    int total_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].result;
    }

    printf("Sumatoria final: %d\n", total_sum);

    return 0;
}

