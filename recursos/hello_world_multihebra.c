#include <pthread.h>
#include <stdio.h>

void *helloworld(void *param) {
    printf("Hello World from thread %ld\n", (long)param);
    pthread_exit((void *)100);
}

int main(int argc, char *argv[]) {
    pthread_t threadsid[5]; // Arreglo de identificadores de threads
    pthread_attr_t attr; // Atributos de los threads
    int i, status;
    pthread_attr_init(&attr); // Inicializa los atributos de los threads
    for (i=0; i<5; i++) {
        pthread_create(&(threadsid[i]), &attr, helloworld, (void *)(long) i); // Crea un nuevo thread 5 veces
    }
    for (i=0; i<5; i++) {
        pthread_join(threadsid[i], (void **)&status); // Espera a que cada thread termine
        printf("Thread %d terminó con estado %d\n", i, status);
    }
    return 0;
}