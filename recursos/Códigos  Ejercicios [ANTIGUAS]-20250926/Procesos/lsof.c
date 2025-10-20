#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipe1[2];  // Pipe entre el proceso 1 y el proceso 2
    int pipe2[2];  // Pipe entre el proceso 2 y el proceso 3

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Error al crear los pipes");
        exit(EXIT_FAILURE);
    }

    pid_t child1, child2;

    // Proceso 1
    if ((child1 = fork()) == -1) {
        perror("Error al crear el proceso 1");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) {  // Código del proceso hijo 1
        close(pipe1[0]);  // Cerramos el extremo de lectura del primer pipe
        dup2(pipe1[1], STDOUT_FILENO);  // Redirigimos la salida estándar al pipe
        close(pipe1[1]);
        while(1);
        execlp("echo", "echo", "Hola desde el proceso 1", NULL);
        perror("Error al ejecutar echo");
        exit(EXIT_FAILURE);
    }

    // Proceso 2
    if ((child2 = fork()) == -1) {
        perror("Error al crear el proceso 2");
        exit(EXIT_FAILURE);
    }

    if (child2 == 0) {  // Código del proceso hijo 2
        close(pipe1[1]);  // Cerramos el extremo de escritura del primer pipe
        dup2(pipe1[0], STDIN_FILENO);  // Redirigimos la entrada estándar desde el pipe1
        close(pipe1[0]);
        close(pipe2[0]);  // Cerramos el extremo de lectura del segundo pipe
        dup2(pipe2[1], STDOUT_FILENO);  // Redirigimos la salida estándar al pipe2
        close(pipe2[1]);
        while(1);
        execlp("tr", "tr", "a-z", "A-Z", NULL);
        perror("Error al ejecutar tr");
        exit(EXIT_FAILURE);
    }

    // Proceso 3
    close(pipe1[0]);  // Cerramos los extremos de lectura y escritura del pipe1 en el proceso padre
    close(pipe1[1]);
    close(pipe2[1]);  // Cerramos el extremo de escritura del segundo pipe
    dup2(pipe2[0], STDIN_FILENO);  // Redirigimos la entrada estándar desde el pipe2
    close(pipe2[0]);
    while(1);
    execlp("cat", "cat", NULL);
    perror("Error al ejecutar cat");
    exit(EXIT_FAILURE);

    return 0;
}
