#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>

#define NUM_VOCALES 5
char vocales[] = {'a', 'e', 'i', 'o', 'u'};

// Función que cuenta una vocal específica en un archivo
int contar_vocal(const char *nombre_archivo, char vocal) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) {
        perror("Error al abrir archivo");
        exit(EXIT_FAILURE);
    }

    int contador = 0;
    char c;
    while ((c = fgetc(archivo)) != EOF) {
        if (tolower(c) == vocal) {
            contador++;
        }
    }

    fclose(archivo);
    return contador;
}

int main() {
    const char *nombre_archivo = "texto.txt";  // nombre del archivo fijo
    int pipes[NUM_VOCALES][2];
    pid_t hijos[NUM_VOCALES];

    // Crear pipes
    for (int i = 0; i < NUM_VOCALES; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error al crear pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Crear procesos hijos
    for (int i = 0; i < NUM_VOCALES; i++) {
        hijos[i] = fork();

        if (hijos[i] < 0) {
            perror("Error al hacer fork");
            exit(EXIT_FAILURE);
        }

        if (hijos[i] == 0) {
            // Proceso hijo
            close(pipes[i][0]); // Cierra lectura

            int cantidad = contar_vocal(nombre_archivo, vocales[i]);
            write(pipes[i][1], &cantidad, sizeof(int));
            close(pipes[i][1]); // Cierra escritura
            exit(0); // Termina el hijo
        } else {
            // Proceso padre
            close(pipes[i][1]); // Cierra escritura
        }
    }

    // Esperar a que terminen los hijos
    for (int i = 0; i < NUM_VOCALES; i++) {
        wait(NULL);
    }

    // Leer resultados y mostrar
    int total = 0;
    printf("Conteo de vocales:\n\n");

    for (int i = 0; i < NUM_VOCALES; i++) {
        int resultado;
        read(pipes[i][0], &resultado, sizeof(int));
        close(pipes[i][0]); // Cierra lectura

        printf("%c: %d\n", vocales[i], resultado);
        total += resultado;
    }

    printf("\nTotal Vocales: %d\n", total);

    return 0;
}
