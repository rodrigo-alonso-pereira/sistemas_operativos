#include "tda_lista_productos.h"
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
    char producto[50]; // Nombre del producto con tamano maximo de 50 caracteres
    int contador;
} data_producto;

// Nodo de la lista de nombres de productos
typedef struct nodo {
    char producto[50];
    struct nodo *siguiente;
} nodo;

// Nodo inicial de la lista
typedef struct {
    nodo *inicio;
    int tamano;
} lista_productos;

// Variables globales
data_producto *lista_paises = NULL; // Lista de paises (asignación dinámica) -> Valor compartido
pthread_mutex_t mutex_paises;       // Mutex para proteger lista_paises

// Función para agregar un producto a la lista
void agregarProducto(lista_productos *lista, const char *nuevo_producto) {
    nodo *nuevo_nodo = (nodo *)malloc(sizeof(nodo)); // Asignar memoria para el nuevo nodo
    strncpy(nuevo_nodo->producto, nuevo_producto, sizeof(nuevo_nodo->producto) - 1); // Copiar el nombre del producto
    nuevo_nodo->producto[sizeof(nuevo_nodo->producto) - 1] = '\0';                   // Asegurar que termine en null
    nuevo_nodo->siguiente = lista->inicio;                                           // Insertar al inicio de la lista
    lista->inicio = nuevo_nodo;                                                      // Actualizar el inicio de la lista
    lista->tamano++; // Incrementar el tamaño de la lista
}

// Funcion para imprimir la lista de productos
void imprimirProductos(lista_productos *lista) {
    nodo *actual = lista->inicio; // Empezar desde el inicio de la lista
    while (actual != NULL) {
        printf("%s\n", actual->producto); // Imprimir el nombre del producto
        actual = actual->siguiente;       // Mover al siguiente nodo
    }
    printf("Total de productos: %d\n", lista->tamano); // Imprimir cantidad de productos
}

char **leerArchivo(char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura

    // Verifica si el archivo se abrió correctamente
    if (!archivo) {
        perror("Error al abrir archivo");
        pthread_mutex_destroy(&mutex_paises); // Destruir mutex antes de salir
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int n_productos = 0;      // Contador de paises
    int capacidad_paises = 0; // Capacidad del array
    int n_filas = 0;          // Numero de filas leidas del archivo

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