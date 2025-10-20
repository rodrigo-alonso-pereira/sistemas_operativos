#include <stdio.h>
#include <stdlib.h> // exit_success
#include <unistd.h> // fork(), getpid()
#include <sys/types.h>
#include <sys/wait.h> //wait()
#include <string.h>
#include <sys/stat.h>

const int valores[] = {0,1,2,3,4,5,6,7,8,9}; // Rangos de valores a contar: [0,1], [2,3], [4,5], [6,7], [8,9]

int contar_numeros(char* nombre_archivo, int valor1, int valor2) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo) {
        perror("Error fopen: ");
        exit(EXIT_FAILURE);
    }
    int contador = 0;
    char valor;
    while ((valor = fgetc(archivo)) != EOF) { // Leer caracter por caracter
        if (valor >= '0' && valor <= '9') { // Verificar que sea un digito en ASCII
            if ((valor - '0') == valor1 || (valor - '0') == valor2) { // Convertir de ASCII a entero
                contador++; 
            }
        }
    }

    fclose(archivo); // Cerrar el archivo
    return contador;
}

int main(int argc, char *argv[1]) {
    if (argc < 2) { // Verificar que se pase el nombre del archivo
        fprintf(stderr, "Error: Nombre de archivo faltante!\n");
        fprintf(stderr, "Se debe iniciar de esta forma: %s <nombre_archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int total_digitos = 0; // Contador total de digitos
    int pipes[5][2]; // 5 pipes, uno por cada rango
    pid_t hijos[5]; // Array para guardar los pids de los hijos

    // Crear pipes
    for (int i=0; i<5; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error pipe: ");
            exit(EXIT_FAILURE);
        }
    }

    // Crear forks
    for (int i=0; i<5; i++) {
        hijos[i] = fork();

        if (hijos[i] < 0) {
            perror("Error fork: ");
            exit(EXIT_FAILURE);
        } else if (hijos[i] == 0) { // Proceso hijo
            close(pipes[i][0]); // Cierra el de lectura y se abre escritura

            int cantidad = contar_numeros(argv[1], valores[i*2], valores[i*2+1]); // [i*2] y [i*2+1] para tomar pares de valores desfasados
            write(pipes[i][1], &cantidad, sizeof(int)); // Escribe la cantidad en el pipe
            close(pipes[i][1]); // Cierra escritura
            exit(EXIT_SUCCESS);
        } else { // Proceso padre
            close(pipes[i][1]); // Cierra escritura
        }
    }

    // Proceso padre
    for (int i=0; i<5; i++) {
        wait(NULL); // Espera a que termine el hijo * 5 veces
    }

    // Leer resultados de los pipes
    printf("Conteo de digitos: \n");
    for (int i=0; i<5; i++) {
        int cantidad;
        read(pipes[i][0], &cantidad, sizeof(int)); // Lee la cantidad del pipe
        printf("%d-%d: %d\n", valores[i*2], valores[i*2+1], cantidad);
        total_digitos += cantidad;
    }
    printf("Total Digitos: %d\n", total_digitos);

    exit(EXIT_SUCCESS);
}