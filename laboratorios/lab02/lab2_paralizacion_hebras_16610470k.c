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
    char pais[50]; // Nombre del pais con tamano maximo de 50 caracteres
    int contador;
} data_pais;

// Variables globales
data_pais *lista_paises = NULL; // Lista de paises (asignación dinámica) -> Valor compartido
int n_paises = 0; // Contador de países
int capacidad_paises = 0; // Capacidad del array
int n_filas = 0; // Numero de filas leidas del archivo
int max_campos = 0; // Maximo de campos por fila
pthread_mutex_t mutex_paises; // Mutex para proteger lista_paises

// Funcion para comparar paises
int comparar_paises(const void *a, const void *b) {
    data_pais *pais_a = (data_pais *)a;
    data_pais *pais_b = (data_pais *)b;
    
    return pais_b->contador - pais_a->contador; // Descendente
}


// Funcion para actualizar lista_paises
void actualizar_lista_paises(const char *name) {
    /*
    Se inicia el mutex ya que se va a trabajar sobre la lista_paises que es un valor compartido 
    entre hebras creadas y asi bloquea el acceso al resto de hebras.
    */ 
    pthread_mutex_lock(&mutex_paises); // Bloquear mutex antes de modificar lista_paises
    for (int i = 0; i < n_paises; i++) {
        // Buscar si el país ya existe en la lista
        if (strcmp(lista_paises[i].pais, name) == 0) {
            lista_paises[i].contador++; // Incrementar contador en el pais
            pthread_mutex_unlock(&mutex_paises); // Desbloquear mutex despues de modificar lista_paises
            return;
        }
    }
    // Si el país no existe, agregar el pais a la lista redimensionando si es necesario
    if (n_paises >= capacidad_paises) {
        capacidad_paises = (capacidad_paises == 0) ? 10 : capacidad_paises * 2; // Asiganr 10 o duplicar capacidad
        lista_paises = realloc(lista_paises, capacidad_paises * sizeof(data_pais)); // Redimensionar
        // Verificar asignación de memoria
        if (!lista_paises) {
            perror("Error al redimensionar lista de paises");
            exit(EXIT_FAILURE);
        }
    }
    strcpy(lista_paises[n_paises].pais, name); // Copiar nombre del país
    lista_paises[n_paises].contador = 1; // Inicializar contador
    n_paises++; // Incrementar el número de países

    /*
    Se cierra el mutex despues de haber modificado la lista_paises para que las otras
    hebras puedan acceder a este recurso compartido.
    */
    pthread_mutex_unlock(&mutex_paises); // Desbloquear mutex despues de modificar lista_paises
}

// Funcion para extraer campos de una linea CSV segun comas y comillas
void extraer_campos_csv(char *linea, char **campos, int max_campos) {
    int num_campos = 0;
    int i = 0;
    int inicio = 0;
    int dentro_comillas = 0;
    
    // Recorrer la linea caracter por caracter
    while (linea[i] != '\0' && num_campos < max_campos) {
        if (linea[i] == '"') { // Si encuentra comillas
            dentro_comillas = !dentro_comillas; // Alternar estado
        } else if (linea[i] == ',' && !dentro_comillas) { // Si encuentra coma fuera de comillas
            linea[i] = '\0'; // Reemplazar coma con fin de string
            campos[num_campos++] = &linea[inicio]; // Guardar inicio del campo
            inicio = i + 1; // Actualizar inicio para el siguiente campo
        }
        i++;
    }
    
    // Último campo
    if (num_campos < max_campos) {
        campos[num_campos++] = &linea[inicio]; // Guardar el último campo
    }
    
    //return num_campos;
}

// Funcion para contar paises
void *leer_pais(void *arg) {
    data_hebras *datos = (data_hebras *)arg; // Convertir el argumento a data_hebras
    // Print de control
    printf("Hebra %d: Procesando paises del %d al %d\n",
           datos->id_hebra,
           datos->inicio,
           datos->fin - 1);
    
    // Recorrer las lineas asignadas a esta hebra
    for (int i = datos->inicio; i < datos->fin; i++) {
        char *campos[10];  // Array para almacenar los campos extraídos
        char linea_copia[1048]; // Buffer para la línea copiada
        
        // Verifica que la línea no sea NULL
        if (datos->lineas[i] == NULL) {
            continue;
        }
        
        strncpy(linea_copia, datos->lineas[i], sizeof(linea_copia) - 1); // Copiar linea original
        linea_copia[sizeof(linea_copia) - 1] = '\0'; // Asegurar el fin de cadena
        
        extraer_campos_csv(linea_copia, campos, 20); // Extraer campos

        // Procesar el campo de país (campo 5) en caso de que sea "India, Francia, ..." para tomar "India"
        if (campos[5][0] == '"') {
            // Remover comillas inicial
            memmove(campos[5], campos[5] + 1, strlen(campos[5]));
            
            // Buscar y eliminar comilla final si existe
            int len = strlen(campos[5]); // Longitud del campo actual
            if (len > 0 && campos[5][len - 1] == '"') { // Si largo > 0 y hay comilla final
                campos[5][len - 1] = '\0'; // Elimina comilla final
            }
            
            // Tomar solo el primer país (hasta la coma)
            char *coma = strchr(campos[5], ','); // Buscar la primera coma
            if (coma) { // Si se encontró la coma
                *coma = '\0'; // Eliminar todo después de la coma
            }
        }
        
        // Actualizar la lista global de países
        if (campos[5][0] != '\0')
            actualizar_lista_paises(campos[5]);
        //printf("Hebra %d - Campo 5: %s\n", datos->id_hebra, campos[5]);
    }

    pthread_exit(NULL);
}

char **leerArchivo(char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura

    // Verifica si el archivo se abrió correctamente
    if (!archivo) {
        perror("Error al abrir archivo");
        exit(EXIT_FAILURE);
    }

    int capacidad = 10; // Capacidad inicial
    char **lineas = malloc(capacidad * sizeof(char *)); // Array para almacenar las líneas

    // Verificar asignación de memoria'
    if (!lineas) { 
        perror("Error al asignar memoria");
        fclose(archivo);
        exit(EXIT_FAILURE);
    }

    char *buffer = NULL; // Buffer temporal para leer cada línea
    size_t len = 0; // Tamaño del buffer
    n_filas = -1; // Inicializar el contador de líneas en -1 para saltar la primera

    // Leer línea por línea
    while (getline(&buffer, &len, archivo) != -1) {
        // Si el n_filas iguala la capacidad, redimensionar el array
        if (n_filas >= capacidad) {
            capacidad *= 2; // Duplicar la capacidad
            char **temp = realloc(lineas, capacidad * sizeof(char *)); // Redimensionar
            // Verificar la nueva asignación de memoria
            if (!temp) { 
                perror("Error al redimensionar memoria");
                for (int i = 0; i < n_filas; i++) {
                    free(lineas[i]);
                }
                free(lineas);
                free(buffer);
                fclose(archivo);
                exit(EXIT_FAILURE);
            }
            lineas = temp; // Actualizar el puntero al nuevo array
        }

        // Eliminar el salto de línea al final si existe \r y/o \n
        buffer[strcspn(buffer, "\n")] = '\0';
        buffer[strcspn(buffer, "\r")] = '\0';

        // Copiar la línea leída al array
        if (n_filas >= 0){ // Saltar la primera línea (encabezado)
            lineas[n_filas] = strdup(buffer); // Duplicar la línea
            // Verificar asignación de memoria
            if (!lineas[n_filas]) {
                perror("Error al duplicar string");
                // Liberar memoria ya asignada
                for (int i = 0; i < n_filas; i++) {
                    free(lineas[i]);
                }
                free(lineas);
                fclose(archivo);
                exit(EXIT_FAILURE);
            }
        }

        n_filas++; // Incrementar el contador de líneas
    }

    free(buffer);    // Liberar el buffer temporal
    fclose(archivo); // Cerrar el archivo

    // Print de control
    printf("Se leyeron %d líneas del archivo\n", n_filas);

    return lineas; // Retornar el array de líneas
}

int main(int argc, char *argv[]) {
    // Verificar que se pasaron los argumentos correctos
    if (argc != 3) { 
        printf("Uso: %s archivo.csv numero_hebras\n", argv[0]);
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

    // Leer el archivo
    char **lineas = leerArchivo(archivo);

    // Print de control
    /*
    for (int i = 0; i < 3; i++) {
        printf("Línea %d: %s\n", i + 1, lineas[i]);
    }
    */
    
    pthread_t hebras[num_hebras];              // Array para las hebras
    data_hebras datos_para_hebras[num_hebras]; // Datos para cada hebra

    // Crear hebras
    for (int i = 0; i < num_hebras; i++) {
        datos_para_hebras[i].lineas = lineas; // Asignar líneas a la hebra
        datos_para_hebras[i].id_hebra = i; // Asignar ID a la hebra
        datos_para_hebras[i].inicio = i * (n_filas / num_hebras); // Calcular inicio
        datos_para_hebras[i].fin = (i + 1) * (n_filas / num_hebras); // Calcular fin
        if (i == num_hebras - 1) {
            datos_para_hebras[i].fin = n_filas; // La última hebra toma el resto
        }
        // Crear la hebra y evalua errores de creacion
        if (pthread_create(&hebras[i], NULL, leer_pais, &datos_para_hebras[i]) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }
    
    // Esperar a que todas las hebras terminen
    for (int i = 0; i < num_hebras; i++) {
        pthread_join(hebras[i], NULL);
        printf("Main: Hebra %d ha terminado.\n", i);
    }

    // Ordenar la lista
    qsort(lista_paises, n_paises, sizeof(data_pais), comparar_paises);

    // Generar archivo con reporte final
    FILE *salida = fopen("reporte_paises_netflix.txt", "w"); // Abrir archivo de salida
    fprintf(salida, "Pais Principal          | Titulos\n"); 
    fprintf(salida, "---------------------------------\n");
    // Escribir los países y sus contadores en el archivo
    for (int i = 0; i < n_paises; i++) {
        fprintf(salida, "%-23s | %d\n", lista_paises[i].pais, lista_paises[i].contador);
    }
    fclose(salida); // Cerrar el archivo de salida
    printf("Reporte generado: reporte_paises_netflix.txt\n");

    // Liberar memoria
    for (int i = 0; i < n_filas; i++) {
        free(lineas[i]);
    }
    free(lineas);
    
    // Liberar lista de países si fue asignada
    if (lista_paises != NULL) {
        free(lista_paises);
    }
    
    // Destruir mutex
    pthread_mutex_destroy(&mutex_paises);
    
    printf("\n...\n");
    printf("fin programa\n");
    return 0;
}