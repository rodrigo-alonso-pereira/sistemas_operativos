#include <stdio.h>
#include <stdlib.h> // exit_success
#include <unistd.h> // fork(), getpid()
#include <sys/types.h>
#include <sys/wait.h> //wait()
#include <string.h>

int main () {
    int fd[2]; // arreglo de 2: [0] lectura, [1] escritura
    pid_t pid; // almacena el  retorno
    char buffer[100]; // Arreglo para guardar string
    char* mensaje_hijo = "Hola papa, soy tu hijo!"; // Mensaje

    pipe(fd); // Crea un pipe de comunicacion

    pid = fork(); // crea un hijo

    if (pid == 0) { //[0] lectura, [1] escritura
        printf("HIJO (PID: %d): Preparando para enviar mensaje...\n", getpid());
        close(fd[0]); // Se cierra el de lectura y se abre el de escritura

        write(fd[1], mensaje_hijo, strlen(mensaje_hijo) + 1);
        printf("HIJO (PID: %d): Mensaje enviado!\n", getpid());
        
        close(fd[1]); // IMPORTANTE: Al momento de terminar la accion cerrar los descriptores utilizados
        printf("HIJO (PID: %d): Terminando.\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        printf("PADRE (PID: %d): Esperando mensaje del hijo (PID: %d)...\n", getpid(), pid);
        close(fd[1]); // Se cierra el de escritura y se abre el de lectura

        read(fd[0], buffer, sizeof(buffer));
        printf("PADRE (PID: %d): Mensaje recibido del hijo: '%s'\n", getpid(), buffer);
        
        close(fd[0]);

        printf("PADRE (PID: %d): Mi hijo ha terminado. Terminando yo.\n", getpid());
    }
    return 0;
}