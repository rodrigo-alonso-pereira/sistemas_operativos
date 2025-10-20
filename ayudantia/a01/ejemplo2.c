#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main () {
    pid_t pid; // almace el retorno del fork
    pid = fork();

    if (pid < 0) { 
        printf("Fallo en fork"); 
        return 1;
    } else if (pid == 0) {
        printf("HIJO: Hola desde el proceso hijo! (PID: %d)\n", getpid());
        printf("HIJO: Terminando. \n");
    } else {
        printf("PADRE: Hola desde el proceso padre! (PID: %d)\n", getpid());
        printf("PADRE: Esperando a que mi hijo (PID: %d) termine...\n", pid);
        wait(NULL); // Espera a que el proceso hijo termine
        printf("PADRE: Mi hijo ha terminado. Terminando yo tambien.\n");
    }
    return 0;
}