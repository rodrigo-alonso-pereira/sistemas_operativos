#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t semaforo_contador; 
#define NUM_RECURSOS 3 
#define NUM_HILOS    5

void *tarea_hilo(void *arg) {
    int id_hilo = *(int*)arg;

    printf("Hebra %d: Esperando para obtener un recurso.\n", id_hilo);

    // WAIT OBTIENE EL RECURSO
    sem_wait(&semaforo_contador);

    printf("Hebra %d: HA OBTENIDO Y LIBERADO EL RECURSO.\n", id_hilo); 

    // POST LIBERA EL RECURSO
    sem_post(&semaforo_contador);
    
    return NULL;
}

int main() {
    pthread_t hilos[NUM_HILOS];
    int ids[NUM_HILOS];

    if (sem_init(&semaforo_contador, 0, NUM_RECURSOS) != 0) {
        perror("Fallo al inicializar el semáforo");
        return 1;
    }

    printf("Simulación iniciada. Recursos disponibles: %d\n\n", NUM_RECURSOS);

    for (int i = 0; i < NUM_HILOS; i++) {
        ids[i] = i + 1;
        if (pthread_create(&hilos[i], NULL, tarea_hilo, &ids[i]) != 0) {
            perror("Fallo al crear el hilo");
            return 1;
        }
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("\nFin.\n");

    sem_destroy(&semaforo_contador);

    return 0;
}