#include <stdio.h> // Habilitar printf o input
#include <unistd.h>
#include <sys/types.h>

int main () {
    pid_t pid_hijo; // Variable que guarda el retorno del fork
    printf("Soy el proceso padre original, mi PID es: %d\n", getpid());

    pid_hijo = fork(); // Fork realiza una copia del padre y corre otro proceso que seria el hijo

    if (pid_hijo < 0) { // Se busca identificar en que proceso se esta
        printf("Error al crear proceso"); 
        return 1;
    } else if (pid_hijo == 0) {
        printf("Soy el proceso hijo, mi PID es: %d y mi padre es: %d\n", getpid(), getppid());
    } else {
        printf("Soy el proceso padre, mi PID es: %d y he creado un hijo con PID: %d\n", getpid(), pid_hijo);
    }
    printf("Mensaje desde el proceso con PID: %d\n", getpid());
    return 0;
}