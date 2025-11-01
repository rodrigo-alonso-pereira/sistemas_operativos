#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4 // Definición de la cantidad de hebras de forma estatica

// Función que ejecutará cada hebra
void *helloworld(void *param) {
    long id = (long)param;
    printf("Hola mundo, soy la hebra %ld\n", id);
    pthread_exit((void *)(id * 100));
}

int main(void) {
    pthread_t threads[NUM_THREADS]; // Arreglo de pthread_t para almacenar las hebras
    int rc; // Código de retorno de las funciones de pthread
    long i; // Índice para los ciclos
    void *retval; // Valor de retorno de las hebras

    // pthread_create para crear hebras
    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_create(&threads[i], NULL, helloworld, (void *)i); // Crear la hebra
        // Error handling
        if (rc != 0) {
            fprintf(stderr, "Error creando hebra %ld: %s\n", i, strerror(rc));
            exit(EXIT_FAILURE);
        }
    }

    // pthread_join     
    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_join(threads[i], &retval); // Esperar a que la hebra termine
        // Error handling
        if (rc != 0) {
            fprintf(stderr, "Error en pthread_join para hebra %ld: %s\n", i, strerror(rc));
            exit(EXIT_FAILURE);
        }
        printf("Main: hebra %ld devolvió %ld\n", i, (long)retval); // Imprimir el valor retornado por la hebra
    }
    // pthread_exit
    printf("fin programa\n");
    return 0;
}



//ptrhread_create:
// - puntero pthread_t: id de la hebra
// puntero a objeto de atributo
// puntero a la funcio
// argumento void *arg