#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define N 100
#define NUM_CHILDREN 4

int main() {
    int vector[N];
    int max_local[NUM_CHILDREN];
    int max_global = 0;
    int i;

    // Inicializar el generador de números aleatorios con una semilla
    srand(time(NULL));

    // Generar el vector de números aleatorios
    for (i = 0; i < N; i++) {
        vector[i] = rand() % 10000;
    }

    // Crear pipes para la comunicación de los máximos locales
    int pipefd[NUM_CHILDREN][2];
    for (i = 0; i < NUM_CHILDREN; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("Error al crear el pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Crear procesos hijos
    for (i = 0; i < NUM_CHILDREN; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) { // Proceso hijo
            close(pipefd[i][0]); // Cerramos el extremo de lectura del pipe en el hijo
            max_local[i] = 0;

            // Encontrar el máximo local en la porción del vector
            //H0 : [2,3...,999]
            //H1 : [23,78,...,998]
            for (int j = i * (N / NUM_CHILDREN); j < (i + 1) * (N / NUM_CHILDREN); j++) {
                if (vector[j] > max_local[i]) {
                    max_local[i] = vector[j];
                }
            }

            // Escribir el máximo local en el pipe
            printf("Hijo: Soy un proceso hijo (PID: %d) y mi max local es: %d\n", getpid(), max_local[i]);
            write(pipefd[i][1], &max_local[i], sizeof(int));
            close(pipefd[i][1]); // Cerramos el extremo de escritura del pipe en el hijo
            exit(EXIT_SUCCESS);
        }
    }

    // Proceso padre
    for (i = 0; i < NUM_CHILDREN; i++) {
        close(pipefd[i][1]); // Cerramos el extremo de escritura del pipe en el padre
        int max_local_child;
        read(pipefd[i][0], &max_local_child, sizeof(int)); // Leer el máximo local de cada hijo
        close(pipefd[i][0]); // Cerramos el extremo de lectura del pipe en el padre

        if (max_local_child > max_global) {
            max_global = max_local_child;
        }
    }

    printf("Padre (PID: %d): El máximo global es %d\n", getpid(),max_global);

    return 0;
}

