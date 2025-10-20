#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Función para crear un árbol binario de procesos
void crearArbol(int nivel, int maxNiveles) {
    if (nivel < maxNiveles) {
        for (int i = 0; i < 2; i++) {
            pid_t child_pid = fork();
            
            if (child_pid == -1) {
                perror("Error al crear el proceso hijo");
                exit(EXIT_FAILURE);
            }
            
            if (child_pid == 0) { // Este es el proceso hijo
                printf("Nivel %d: Proceso hijo %d (Padre: %d)\n", nivel, getpid(), getppid());
                crearArbol(nivel + 1, maxNiveles);
                //exit(EXIT_SUCCESS);
                while(1);
            }
        }
    }
}

int main() {
    int maxNiveles = 3;

    printf("Nivel 0: Proceso padre %d\n", getpid());
    crearArbol(1, maxNiveles);

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < 2 * (2 * maxNiveles - 1); i++) {
        wait(NULL);
    }

    printf("Proceso padre %d: Todos los procesos hijos han terminado.\n", getpid());
    while(1);
    return 0;
}
