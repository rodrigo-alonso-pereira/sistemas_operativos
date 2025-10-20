#include <stdio.h>
#include <pthread.h>

// Función que será ejecutada por la primera hebra
void *funcion_hebra1(void *arg) {
    for (int i = 1; i <= 10; i++) {
        printf("Hebra 1, Iteración %d\n", i);
    }
    pthread_exit(NULL);
}

// Función que será ejecutada por la segunda hebra
void *funcion_hebra2(void *arg) {
    for (int i = 1; i <= 10; i++) {
        printf("Hebra 2, Iteración %d\n", i);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hebra1, hebra2;

    // Crear la primera hebra
    if (pthread_create(&hebra1, NULL, funcion_hebra1, NULL)) {
        fprintf(stderr, "Error al crear la hebra 1\n");
        return 1;
    }

    // Crear la segunda hebra
    if (pthread_create(&hebra2, NULL, funcion_hebra2, NULL)) {
        fprintf(stderr, "Error al crear la hebra 2\n");
        return 1;
    }

    // Esperar a que ambas hebras terminen
    pthread_join(hebra1, NULL);
    pthread_join(hebra2, NULL);

    printf("Ambas hebras han terminado.\n");

    return 0;
}

