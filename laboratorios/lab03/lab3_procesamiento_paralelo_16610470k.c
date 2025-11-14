#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Estructuras para pasar datos a las hebras
typedef struct {
    char **lineas;
    int inicio;
    int fin;
    int id_hebra;
} data_hebras;

// Estructura para contar paises
typedef struct {
    char producto[50]; // Nombre del pais con tamano maximo de 50 caracteres
    int contador;
} data_producto;

// Variables globales
data_producto *lista_paises = NULL; // Lista de paises (asignación dinámica) -> Valor compartido
int n_productos = 0; // Contador de paises
int capacidad_paises = 0; // Capacidad del array
int n_filas = 0; // Numero de filas leidas del archivo
pthread_mutex_t mutex_paises; // Mutex para proteger lista_paises

char **leerArchivo(char *nombre_archivo) {
    
}

int main(int argc, char *argv[]) {
    // Verificar que se pasaron los argumentos correctos
    if (argc != 3) { 
        printf("Uso: %s nombre_archivo.txt numero_hebras\n", argv[0]);
        return 1;
    }

    char *archivo = argv[1];        // Nombre del archivo CSV
    int num_hebras = atoi(argv[2]); // Número de hebras a utilizar

    // Valida que el numero de hebras sea mayor a 0
    if (num_hebras <= 0) {
        fprintf(stderr, "Numero de hebras invalido\n");
        return 1;
    }

    // Inicializar mutex
    if (pthread_mutex_init(&mutex_paises, NULL) != 0) {
        fprintf(stderr, "Error al inicializar mutex\n");
        return 1;
    }

    // Leer el archivo y asignar valores a arreglo de string
    char **lineas = leerArchivo(archivo);

}