#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

pthread_barrier_t barrera;

void* tarea_hebra(void* arg) {
    int id = *(int*)arg;

    printf("Hebra %d: Realizando tarea independiente\n", id);

    // pthread_barrier_wait(&barrera); // Espera en la barrera hasta que todas las hebras lleguen aquí

    printf("Hebra %d: Continuando con la siguiente tarea\n", id);

    pthread_exit(NULL);
}

int main() {
    pthread_t hebras[NUM_THREADS];
    int ids[NUM_THREADS];

    pthread_barrier_init(&barrera, NULL, NUM_THREADS); // Inicializa la barrera para NUM_THREADS hebras

    for (int i = 0; i < NUM_THREADS; ++i) {
        ids[i] = i;
        // Crear cada hebra con condición de error
        if (pthread_create(&hebras[i], NULL, tarea_hebra, &ids[i]) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }
    
    // Esperar a que todas las hebras terminen
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(hebras[i], NULL);
    } 
    // Destruir la barrera
    pthread_barrier_destroy(&barrera);

    return 0;
}

