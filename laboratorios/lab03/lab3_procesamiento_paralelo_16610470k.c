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
    char producto[50];
    int contador;
} data_producto;

// Nodo de la lista de nombres de productos
typedef struct nodo {
    char *producto;         // Nombre del producto (con tamaño dinámico)
    struct nodo *siguiente; // Puntero al siguiente nodo
} nodo;

// Nodo inicial de la lista
typedef struct {
    nodo *inicio; // Puntero al primer nodo de la lista
    int tamano;   // Tamaño de la lista
} lista_productos;

// Variables globales
pthread_mutex_t mutex_productos; // Mutex para proteger lista_paises

// Función para agregar un producto a la lista
void agregarProducto(lista_productos *lista, const char *nuevo_producto) {
    nodo *nuevo_nodo = (nodo *)malloc(sizeof(nodo)); // Asignar memoria para el nuevo nodo
    if (!nuevo_nodo) {
        perror("Error al asignar memoria para el nuevo nodo");
        return;
    }

    int largo_producto = strlen(nuevo_producto) + 1;                      // Calcular el largo del nombre producto
    nuevo_nodo->producto = (char *)malloc(largo_producto * sizeof(char)); // Asignar memoria nombre producto
    if (!nuevo_nodo->producto) {
        perror("Error al asignar memoria para el nombre del producto");
        free(nuevo_nodo);
        return;
    }
    strcpy(nuevo_nodo->producto, nuevo_producto); // Copiar el nombre del producto
    nuevo_nodo->siguiente = lista->inicio;        // Insertar al inicio de la lista
    lista->inicio = nuevo_nodo;                   // Actualizar el inicio de la lista
    lista->tamano++;                              // Incrementar el tamaño de la lista
}

// Funcion para imprimir la lista de productos
void imprimirProductos(lista_productos *lista) {
    nodo *actual = lista->inicio; // Empezar desde el inicio de la lista
    while (actual != NULL) {
        printf("%s", actual->producto); // Imprimir el nombre del producto
        actual = actual->siguiente;     // Mover al siguiente nodo
    }
    printf("Total de productos: %d\n", lista->tamano); // Imprimir cantidad de productos
}

void liberarListaProductos(lista_productos *lista) {
    nodo *actual = lista->inicio; // Empezar desde el inicio de la lista
    nodo *siguiente;              // Nodo para el siguiente elemento

    while (actual != NULL) {
        siguiente = actual->siguiente; // Guardar el nodo actual
        free(actual->producto);        // Liberar el nombre del producto
        free(actual);                  // Liberar el nodo
        actual = siguiente;            // Mover al siguiente nodo
    }
    lista->inicio = NULL; // Establecer el inicio a NULL
    lista->tamano = 0;    // Establecer el tamaño a 0
}

char **leerArchivo(char *nombre_archivo, lista_productos *lista) {
    char *buffer = NULL;                        // Puntero para el buffer de lectura
    size_t tamano_buffer = 0;                   // tamaño del buffer
    FILE *archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura

    // Verifica si el archivo se abrió correctamente
    if (!archivo) {
        perror("Error al abrir archivo");
        pthread_mutex_destroy(&mutex_productos); // Destruir mutex antes de salir
        exit(EXIT_FAILURE);
    }

    while (getline(&buffer, &tamano_buffer, archivo) != -1) { // Leer línea sin límite de tamaño
        // int largo = strlen(buffer);
        // printf("Largo: %d -> %s", largo, buffer); // Imprimir la línea leída para depuración
        agregarProducto(lista, buffer); // Agregar el producto a la lista
    }

    free(buffer);    // Liberar el buffer
    fclose(archivo); // Cerrar el archivo
    return NULL;
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

    lista_productos lista; // Crear la lista de productos
    lista.inicio = NULL;   // Inicializar el inicio de la lista
    lista.tamano = 0;      // Inicializar el tamaño de la lista

    // Inicializar mutex
    if (pthread_mutex_init(&mutex_productos, NULL) != 0) {
        fprintf(stderr, "Error al inicializar mutex\n");
        return 1;
    }

    // Leer el archivo y asignar valores a arreglo de string
    char **lineas = leerArchivo(archivo, &lista);

    /*
    La lista se imprime de forma inversa debido a que los nuevos nodos se agregan al inicio de la lista.
    */
    // imprimirProductos(&lista); // Imprimir la lista de productos (depuración)

    pthread_t hebras[num_hebras];              // Array para las hebras
    data_hebras datos_para_hebras[num_hebras]; // Datos para cada hebra
}