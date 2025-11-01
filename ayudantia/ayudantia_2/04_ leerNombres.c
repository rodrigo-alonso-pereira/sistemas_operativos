#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define NUM_NOMBRES 10
#define NUM_HEBRAS 2
#define NOMBRES_POR_HEBRA (NUM_NOMBRES / NUM_HEBRAS)

typedef struct {
    char** nombres;
    int inicio;
    int fin;
    int id_hebra;
} datos_hebra_t;

void* imprimir_nombres(void* arg) {
    datos_hebra_t* datos = (datos_hebra_t*)arg;
    printf("Hebra %d: Procesando nombres del %d al %d\n", datos->id_hebra, datos->inicio, datos->fin - 1);
    for (int i = datos->inicio; i < datos->fin; i++) {
        if (datos->nombres[i] != NULL) {
            printf("Hebra %d: Nombre -> %s\n", datos->id_hebra, datos->nombres[i]);
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hebras[NUM_HEBRAS];
    datos_hebra_t datos_para_hebras[NUM_HEBRAS];
    char* nombres[NUM_NOMBRES] = {NULL};
    char buffer[100];
    FILE* archivo;
    int i;

    archivo = fopen("nombres.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir archivo");
        return 1;
    }

    printf("Leyendo nombres del archivo...\n");
    for (i = 0; i < NUM_NOMBRES; i++) {
        if (fgets(buffer, sizeof(buffer), archivo) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            nombres[i] = strdup(buffer);
        }
    }
    fclose(archivo);
    printf("Nombres leidos\n\n");

    for (i = 0; i < NUM_HEBRAS; i++) {
        datos_para_hebras[i].nombres = nombres;
        datos_para_hebras[i].id_hebra = i;
        datos_para_hebras[i].inicio = i * NOMBRES_POR_HEBRA;
        datos_para_hebras[i].fin = (i + 1) * NOMBRES_POR_HEBRA;

        if (i == NUM_HEBRAS - 1 || NOMBRES_POR_HEBRA == 0) {
            datos_para_hebras[i].fin = NUM_NOMBRES;
        }

        printf("main: crea hebra %d para procesar desde el índice %d hasta %d\n",
               i, datos_para_hebras[i].inicio, datos_para_hebras[i].fin - 1);

        pthread_create(&hebras[i],
                       NULL,
                       imprimir_nombres,
                       &datos_para_hebras[i]);
    }

    for (i = 0; i < NUM_HEBRAS; i++) {
        pthread_join(hebras[i], NULL);
        printf("Main: Hebra %d ha terminado.\n", i);
    }

    printf("fin programa\n");
    return 0;
}



