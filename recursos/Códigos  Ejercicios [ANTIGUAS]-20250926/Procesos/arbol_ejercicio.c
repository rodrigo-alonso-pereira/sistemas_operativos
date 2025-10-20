#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("Raíz del árbol de procesos (PID %d)\n", getpid());

    for (int i = 0; i < 2; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Error al crear el proceso hijo");
            return 1;
        }

        if (child_pid == 0) { // Proceso hijo
            printf("Proceso Hijo (PID %d) - PID del Padre (PPID %d)\n", getpid(), getppid());

            for (int j = 0; j < 3; j++) {
                pid_t grandchild_pid = fork();

                if (grandchild_pid == -1) {
                    perror("Error al crear el proceso nieto");
                    return 1;
                }

                if (grandchild_pid == 0) { // Proceso nieto
                    printf("Proceso Nieto (PID %d) - PID del Padre (PPID %d)\n", getpid(), getppid());
                    return 0;
                }
            }

            // El proceso hijo no crea más procesos, sale del bucle
            return 0;
        }
    }
    // Esperar a que todos los procesos hijos terminen (solo el padre llega aquí)
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    return 0;
}

