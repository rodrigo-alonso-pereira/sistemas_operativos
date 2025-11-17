#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Estructura para contar productos
typedef struct data_producto {
    char *producto;                  // Nombre del producto (con tamaño dinámico)
    int contador;                    // Contador de unidades vendidas
    struct data_producto *siguiente; // Puntero al siguiente nodo
} data_producto;

// Nodo de la lista de nombres de productos
typedef struct nodo {
    char *producto;         // Nombre del producto (con tamaño dinámico)
    struct nodo *siguiente; // Puntero al siguiente nodo
} nodo;

// Nodo inicial de la lista de nombres de productos
typedef struct {
    nodo *inicio; // Puntero al primer nodo de la lista
    int tamano;   // Tamaño de la lista
} lista_nombre_productos;

// Nodo inicial de la lista de productos resumen
typedef struct {
    data_producto *inicio; // Puntero al primer nodo de la lista de productos resumen
    int tamano;            // Tamaño de la lista
} lista_resumen_productos;

// Estructuras para pasar datos a las hebras
typedef struct {
    lista_nombre_productos *lista; // Puntero a la lista de nombres de productos
    int inicio;                    // Índice de inicio
    int fin;                       // Índice de fin
    int id_hebra;                  // Identificador de la hebra
} data_hebras;

// Variables globales
pthread_mutex_t mutex_productos;       // Mutex para proteger lista_productos
pthread_barrier_t barrera;             // Barrera para sincronización de hebras
lista_resumen_productos lista_resumen; // Crear la lista de productos resumen

// Función para agregar un producto a la lista
void agregar_nombre_producto(lista_nombre_productos *lista, const char *nuevo_producto) {
    nodo *nuevo_nodo = (nodo *)malloc(sizeof(nodo)); // Asignar memoria para el nuevo nodo
    // Verificar si la asignación de memoria fue exitosa
    if (!nuevo_nodo) {
        perror("Error al asignar memoria para el nuevo nodo");
        return;
    }

    int largo_producto = strlen(nuevo_producto) + 1;                      // Calcular el largo del nombre producto
    nuevo_nodo->producto = (char *)malloc(largo_producto * sizeof(char)); // Asignar memoria nombre producto

    // Verificar si la asignación de memoria fue exitosa
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

// Funcion para imprimir la lista de nombre de productos
void imprimir_nombre_productos(lista_nombre_productos *lista) {
    nodo *actual = lista->inicio; // Empezar desde el inicio de la lista

    // Recorrer la lista e imprimir cada producto
    while (actual != NULL) {
        printf("%s", actual->producto); // Imprimir el nombre del producto
        actual = actual->siguiente;     // Mover al siguiente nodo
    }
    printf("Total de productos: %d\n", lista->tamano); // Imprimir cantidad de productos
}

// Funcion para imprimir la lista de productos
void imprimir_resumen_productos(lista_resumen_productos *lista) {
    printf("\nResumen Ventas Cyber Day - 2025:\n");
    data_producto *actual = lista->inicio; // Empezar desde el inicio de la lista

    // Recorrer la lista e imprimir cada producto y su contador
    while (actual != NULL) {
        printf("%s: %d unidades\n", actual->producto,
               actual->contador);   // Imprimir el nombre del producto y su contador
        actual = actual->siguiente; // Mover al siguiente nodo
    }
    printf("Total de productos: %d\n", lista->tamano); // Imprimir cantidad de productos
}

// Función para liberar la memoria de la lista de nombre de productos
void liberar_nombre_productos(lista_nombre_productos *lista) {
    nodo *actual = lista->inicio; // Empezar desde el inicio de la lista
    nodo *siguiente;              // Nodo para el siguiente elemento

    // Recorrer la lista y liberar cada nodo
    while (actual != NULL) {
        siguiente = actual->siguiente; // Guardar el nodo actual
        free(actual->producto);        // Liberar el nombre del producto
        free(actual);                  // Liberar el nodo
        actual = siguiente;            // Mover al siguiente nodo
    }
    lista->inicio = NULL; // Establecer el inicio a NULL
    lista->tamano = 0;    // Establecer el tamaño a 0
}

// Función para liberar la memoria de la lista resumen de productos
void liberar_resumen_productos(lista_resumen_productos *lista) {
    data_producto *actual = lista->inicio; // Empezar desde el inicio de la lista
    data_producto *siguiente;              // Nodo para el siguiente elemento

    while (actual != NULL) {
        siguiente = actual->siguiente; // Guardar el nodo actual
        free(actual->producto);        // Liberar el nombre del producto
        free(actual);                  // Liberar el nodo
        actual = siguiente;            // Mover al siguiente nodo
    }
    lista->inicio = NULL; // Establecer el inicio a NULL
    lista->tamano = 0;    // Establecer el tamaño a 0
}

// Función para actualizar la lista de productos resumen
void actualizar_lista_productos(const char *name) {
    pthread_mutex_lock(&mutex_productos);
    data_producto *producto_actual = lista_resumen.inicio; // Empezar desde el inicio de la lista

    // Buscar si el producto ya existe en la lista
    while (producto_actual != NULL) {
        // Comparar nombres de productos
        if (strcmp(producto_actual->producto, name) == 0) {
            producto_actual->contador++;            // Incrementar el contador si el producto ya existe
            pthread_mutex_unlock(&mutex_productos); // Desbloquear mutex despues de modificar lista_paises
            return;
        }
        producto_actual = producto_actual->siguiente; // Mover al siguiente nodo
    }

    // Si el producto no existe, se crea un nuevo nodo para el producto y se agrega a la lista
    data_producto *nuevo_producto =
        (data_producto *)malloc(sizeof(data_producto)); // Asignar memoria para el nuevo producto

    // Verificar si la asignación de memoria fue exitosa
    if (!nuevo_producto) {
        perror("Error al asignar memoria para nuevo producto");
        pthread_mutex_unlock(&mutex_productos); // Desbloquear mutex antes de salir
        return;
    }

    int largo_nombre_producto = strlen(name) + 1; // Calcular el largo del nombre del producto
    nuevo_producto->producto =
        (char *)malloc(largo_nombre_producto * sizeof(char)); // Asignar memoria para el nombre del producto

    // Verificar si la asignación de memoria fue exitosa
    if (!nuevo_producto->producto) {
        perror("Error al asignar memoria para nombre del nuevo producto");
        free(nuevo_producto);
        pthread_mutex_unlock(&mutex_productos); // Desbloquear mutex antes de salir
        return;
    }

    strcpy(nuevo_producto->producto, name);           // Copiar el nombre del producto
    nuevo_producto->contador = 1;                     // Inicializar el contador en 1
    nuevo_producto->siguiente = lista_resumen.inicio; //
    lista_resumen.inicio = nuevo_producto;            // Agregar el nuevo producto al inicio de la lista
    lista_resumen.tamano++;                           // Incrementar el tamaño de la lista
    pthread_mutex_unlock(&mutex_productos);           // Desbloquear mutex antes de salir
}

// Función que será ejecutada por cada hebra
void *leer_producto(void *arg) {
    data_hebras *datos = (data_hebras *)arg; // Convertir el argumento a data_hebras
    // Print de control
    printf("Hebra %d: Procesando paises del %d al %d\n", datos->id_hebra, datos->inicio, datos->fin - 1);

    nodo *nodo_actual = datos->lista->inicio; // Empezar desde el inicio de la lista
    // printf("Hebra %d: Nodo inicial producto: %s\n", datos->id_hebra, nodo_actual->producto); // Print de control
    int contador = 0; // Contador para rastrear la posición en la lista

    // Recorrer la lista de productos
    while (nodo_actual != NULL) {
        if (contador >= datos->inicio && contador < datos->fin) { // Si el contador está en el rango asignado a la hebra
            // printf("Hebra %d: Procesando producto: %s, numero %d\n", datos->id_hebra, nodo_actual->producto,
            // contador); // Print de control
            actualizar_lista_productos(nodo_actual->producto); // Actualizar la lista de productos
        }

        nodo_actual = nodo_actual->siguiente; // Mover al siguiente nodo
        contador++;                           // Incrementar el contador

        // Salir del bucle si se ha alcanzado el final asignado
        if (contador >= datos->fin) {
            break; // Salir del bucle
        }
    }
    // printf("Hebra %d: Estoy esperando que todas terminen.\n", datos->id_hebra); // Print de control
    pthread_barrier_wait(&barrera); // Esperar a que todas las hebras lleguen a la barrera
    pthread_exit(NULL);             // Terminar la hebra
}

// Función para leer el archivo y llenar la lista de nombres de productos
void leer_archivo(char *nombre_archivo, lista_nombre_productos *lista) {
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
        // Quitar saltos de línea
        buffer[strcspn(buffer, "\n")] = '\0';
        buffer[strcspn(buffer, "\r")] = '\0';

        // Quitar espacios del FINAL (usando isspace)
        int i = strlen(buffer) - 1;
        while (i >= 0 && isspace((unsigned char)buffer[i])) {
            buffer[i] = '\0';
            i--;
        }

        // Quitar espacios del INICIO
        char *start = buffer;
        while (isspace((unsigned char)*start)) {
            start++;
        }

        // Quitar el BOM de UTF-8
        if (strncmp(start, "\xEF\xBB\xBF", 3) == 0) {
            start += 3; // "Saltamos" los 3 bytes del BOM
        }

        // Solo agregar si el string resultante tiene contenido
        if (strlen(start) > 0) {
            agregar_nombre_producto(lista, start);
        }
    }

    free(buffer);    // Liberar el buffer
    fclose(archivo); // Cerrar el archivo
}

void crear_archivo_salida(const char *nombre_archivo, lista_resumen_productos *lista) {
    FILE *archivo = fopen(nombre_archivo, "w"); // Abrir el archivo en modo escritura

    // Verifica si el archivo se abrió correctamente
    if (!archivo) {
        perror("Error al crear archivo de salida");
        return;
    }
    fprintf(archivo, "Resultado Ventas Cyber Day - 2025:\n\n");
    data_producto *actual = lista->inicio; // Empezar desde el inicio de la lista

    // Recorrer la lista e imprimir cada producto y su contador
    while (actual != NULL) {
        fprintf(archivo, "%s: %d unidades\n", actual->producto,
                actual->contador);  // Imprimir el nombre del producto y su contador
        actual = actual->siguiente; // Mover al siguiente nodo
    }

    // Escribir encabezado
    fclose(archivo); // Cerrar el archivo de salida
    printf("Reporte generado: reporte_ventas_final.txt\n");
}

void consulta_resultados(lista_resumen_productos *lista) {
    char producto_consulta[256]; // Buffer para el nombre del producto
    printf("\n--------------------------------------------------------------\n");
    printf("Consulta de ventas por producto (escriba 'salir' para terminar):");
    printf("\n--------------------------------------------------------------\n");
    while (1) {
        printf("Ingrese el nombre del producto para ver sus unidades vendidas: ");
        fgets(producto_consulta, sizeof(producto_consulta), stdin); // Leer el nombre del producto
        producto_consulta[strcspn(producto_consulta, "\n")] = '\0'; // Quitar salto de línea

        // Verificar si el usuario quiere salir
        if (strcmp(producto_consulta, "salir") == 0) {
            break;
        }

        data_producto *actual = lista->inicio; // Empezar desde el inicio de la lista
        int encontrado = 0;                    // Bandera para indicar si se encontró el producto

        // Recorrer la lista para buscar el producto
        while (actual != NULL) {
            if (strcmp(actual->producto, producto_consulta) == 0) {
                printf("El producto '%s' vendió %d unidades.\n", actual->producto, actual->contador);
                encontrado = 1; // Marcar como encontrado
                break;
            }
            actual = actual->siguiente; // Mover al siguiente nodo
        }

        if (!encontrado) {
            printf("El producto '%s' no fue encontrado en el resumen de ventas.\n", producto_consulta);
        }
    }
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

    // Crear e inicializar listas
    lista_nombre_productos lista; // Crear la lista de nombre de productos
    lista.inicio = NULL;          // Inicializar el inicio de la lista
    lista.tamano = 0;             // Inicializar el tamaño de la lista

    lista_resumen.inicio = NULL; // Inicializar el puntero de productos
    lista_resumen.tamano = 0;    // Inicializar el tamaño de la lista

    // Inicializar mutex
    if (pthread_mutex_init(&mutex_productos, NULL) != 0) {
        fprintf(stderr, "Error al inicializar mutex\n");
        return 1;
    }

    // Leer el archivo y asignar valores a arreglo de string
    leer_archivo(archivo, &lista);

    /*
    La lista se imprime de forma inversa debido a que los nuevos nodos se agregan al inicio de la lista.
    */
    // imprimir_nombre_productos(&lista); // Imprimir la lista de productos (depuración)

    pthread_t hebras[num_hebras];              // Array para las hebras
    data_hebras datos_para_hebras[num_hebras]; // Datos para cada hebra
    // Inicializar la barrera
    pthread_barrier_init(&barrera, NULL, num_hebras); // +1 para la hebra principal

    // Crear las hebras
    for (int i = 0; i < num_hebras; i++) {
        datos_para_hebras[i].lista = &lista;                           // Pasar la lista de productos
        datos_para_hebras[i].id_hebra = i;                             // ID de la hebra
        datos_para_hebras[i].inicio = i * (lista.tamano / num_hebras); // Calcular inicio
        datos_para_hebras[i].fin =
            (i == num_hebras - 1) ? lista.tamano : (i + 1) * (lista.tamano / num_hebras); // Calcular fin

        // Crear la hebra y verificar errores
        if (pthread_create(&hebras[i], NULL, leer_producto, (void *)&datos_para_hebras[i]) != 0) {
            fprintf(stderr, "Error al crear hebra %d\n", i);
            pthread_mutex_destroy(&mutex_productos); // Destruir mutex antes de salir
            liberar_nombre_productos(&lista);        // Liberar la lista antes de salir
            return 1;
        }
    }

    // Finalizar las hebras
    for (int i = 0; i < num_hebras; i++) {
        pthread_join(hebras[i], NULL);
        printf("Main: Hebra %d ha terminado.\n", i);
    }

    // imprimir_resumen_productos(&lista_resumen); // Imprimir la lista resumen de productos (depuración)
    crear_archivo_salida("reporte_ventas_final.txt", &lista_resumen); // Crear archivo de salida

    // Consulta resultados por consola
    consulta_resultados(&lista_resumen);

    // Liberar recursos
    pthread_mutex_destroy(&mutex_productos);   // Destruir mutex
    liberar_nombre_productos(&lista);          // Liberar la lista de nombre de productos
    liberar_resumen_productos(&lista_resumen); // Liberar la lista resumen de productos

    // Final del programa
    printf("\n...\n");
    printf("fin programa\n");
    return 0;
}