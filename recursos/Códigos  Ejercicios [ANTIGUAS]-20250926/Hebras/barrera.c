#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

// Declaración e inicialización de la barrera
pthread_barrier_t barrera;

void* tarea_hebra(void* arg) {
    int id = *(int*)arg;

    // Fase 1: Realizar alguna tarea independiente en cada hebra
    printf("Hebra %d: Realizando tarea independiente\n", id);

    // Barrera: Todas las hebras deben esperar aquí hasta que todas alcancen este punto
    pthread_barrier_wait(&barrera);

    // Fase 2: Continuar con la siguiente tarea después de la barrera
    printf("Hebra %d: Continuando con la siguiente tarea\n", id);

    pthread_exit(NULL);
}

int main() {
    pthread_t hebras[NUM_THREADS];
    int ids[NUM_THREADS];

    // Inicializar la barrera con el número de hebras esperadas
    pthread_barrier_init(&barrera, NULL, NUM_THREADS);

    // Crear hebras
    for (int i = 0; i < NUM_THREADS; ++i) {
        ids[i] = i;
        if (pthread_create(&hebras[i], NULL, tarea_hebra, &ids[i]) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }

    // Esperar a que todas las hebras terminen
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(hebras[i], NULL);
    }

    // Destruir la barrera después de su uso
    pthread_barrier_destroy(&barrera);

    return 0;
}

