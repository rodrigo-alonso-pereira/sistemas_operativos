#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

pthread_barrier_t barrera;

void* tarea_hebra(void* arg) {
    int id = *(int*)arg;

    printf("Hebra %d: Realizando tarea independiente\n", id);

    pthread_barrier_wait(&barrera);

    printf("Hebra %d: Continuando con la siguiente tarea\n", id);

    pthread_exit(NULL);
}

int main() {
    pthread_t hebras[NUM_THREADS];
    int ids[NUM_THREADS];

    pthread_barrier_init(&barrera, NULL, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        ids[i] = i;
        if (pthread_create(&hebras[i], NULL, tarea_hebra, &ids[i]) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(hebras[i], NULL);
    }
    pthread_barrier_destroy(&barrera);

    return 0;
}

