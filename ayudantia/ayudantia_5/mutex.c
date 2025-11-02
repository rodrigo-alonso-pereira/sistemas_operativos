#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5
#define ITERATIONS 10000

// contador_compartido
int contador_compartido = 0;

pthread_mutex_t mutex_contador;

void* tarea_hebra(void* arg) {
    for (int i = 0; i < ITERATIONS; ++i) {

        pthread_mutex_lock(&mutex_contador);
        
        contador_compartido++; 

        pthread_mutex_unlock(&mutex_contador);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t hebras[NUM_THREADS];

    if (pthread_mutex_init(&mutex_contador, NULL) != 0) {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return 1;
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_create(&hebras[i], NULL, tarea_hebra, NULL) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(hebras[i], NULL);
    }

    pthread_mutex_destroy(&mutex_contador);

    printf("Valor final del contador compartido: %d\n", contador_compartido);

    return 0;
}