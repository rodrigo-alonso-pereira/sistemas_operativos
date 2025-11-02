#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_NOMBRES 40000
#define NUM_HEBRAS 100
#define NOMBRES_POR_HEBRA (NUM_NOMBRES / NUM_HEBRAS)

int contador_nombres_compartido = 0;
pthread_mutex_t mutex_contador_nombres;

typedef struct {
    char** nombres;
    int inicio;
    int fin;
    int id_hebra;
} datos_hebra_t;

void* contar_nombres(void* arg) {
    datos_hebra_t* datos = (datos_hebra_t*)arg;

    printf("Hebra %d: Procesando nombres del %d al %d\n", datos->id_hebra, datos->inicio, datos->fin - 1);

    for (int i = datos->inicio; i < datos->fin; i++) {
        if (datos->nombres[i] != NULL) {
            printf("Hebra %d: Nombre -> %s\n", datos->id_hebra, datos->nombres[i]);
            
            pthread_mutex_lock(&mutex_contador_nombres);

            // SC
            contador_nombres_compartido++;
            // SC
            
            pthread_mutex_unlock(&mutex_contador_nombres);
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

    // mutex
    if (pthread_mutex_init(&mutex_contador_nombres, NULL) != 0) {
        perror("Error al inicializar el mutex");
        return 1;
    }

    archivo = fopen("nombres.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir archivo");
        pthread_mutex_destroy(&mutex_contador_nombres);
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

        if (pthread_create(&hebras[i], NULL, contar_nombres, &datos_para_hebras[i]) != 0) {
            perror("Error al crear la hebra");

            for (int j = 0; j < i; j++) {
                pthread_join(hebras[j], NULL);
            }

            for (int k = 0; k < NUM_NOMBRES; ++k) {
                if (nombres[k]) free(nombres[k]);
            }
            return 1;
        }
    }

    for (i = 0; i < NUM_HEBRAS; i++) {
        pthread_join(hebras[i], NULL);
        printf("Main: Hebra %d ha terminado.\n", i);
    }

    pthread_mutex_destroy(&mutex_contador_nombres);

    printf("\nTotal de nombres contados por las hebras: %d\n", contador_nombres_compartido);

    return 0;
}