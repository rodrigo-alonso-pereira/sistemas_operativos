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
    char pais[50];
    int contador;
} data_pais;

// Variables globales
data_pais lista_paises[0]; // Lista de paises (inicialmente vacia)
int n_filas = 0; // Numero de filas leidas del archivo
int max_campos = 0;

// TODO: Agregar funcion para agregado dinamico de data_pais en caso de no existir en lista general

// Funcion para extraer campos de una linea CSV segun comas y comillas
int extraer_campos_csv(char *linea, char **campos, int max_campos) {
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
    
    return num_campos;
}

// Funcion para contar paises
void *leer_pais(void *arg) {
    data_hebras *datos = (data_hebras *)arg; // Convertir el argumento a data_hebras
    // print de test
    printf("Hebra %d: Procesando paises del %d al %d\n",
           datos->id_hebra,
           datos->inicio,
           datos->fin - 1);
    
    for (int i = datos->inicio; i < datos->fin; i++) {
        char *campos[12];  
        char linea_copia[1024];
        strcpy(linea_copia, datos->lineas[i]);
        
        // TODO: Hacer que 20 sea dinamico segun primera fila de .csv
        int num_campos = extraer_campos_csv(linea_copia, campos, 20);

        if (campos[5][0] == '"') {
            int i = 1;
            int num_pais = 0;
            
            while (campos[5][i] != '\0' && num_pais < 1) {
                if (campos[5][i] == ',') {
                    campos[5][i] = '\0';
                    campos[5] = &campos[5][1];
                    num_pais++;
                }
                i++;
            }
        }
        // TODO: Agregar condicion para contar pais
        printf("[CONTROL] Hebra %d - Campo 5: %s\n", datos->id_hebra, campos[5]);
    }

    pthread_exit(NULL);
}

char **leerArchivo(char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura
    if (!archivo) {
        perror("Error al abrir archivo");
        exit(EXIT_FAILURE);
    }

    int capacidad = 10; // Capacidad inicial
    char **lineas = malloc(capacidad * sizeof(char *)); // Array para almacenar las líneas
    if (!lineas) { // Verificar asignación de memoria
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
            if (!temp) { // Verificar la nueva asignación de memoria
                perror("Error al redimensionar memoria");
                for (int i = 0; i < n_filas; i++) {
                    free(lineas[i]);
                }
                free(lineas);
                free(buffer);
                fclose(archivo);
                exit(EXIT_FAILURE);
            }
            lineas = temp;
        }

        // Eliminar el salto de línea al final si existe \r y/o \n
        buffer[strcspn(buffer, "\n")] = '\0';
        buffer[strcspn(buffer, "\r")] = '\0';

        // Copiar la línea leída al array
        if (n_filas >= 0){ // Saltar la primera línea (encabezado)
            lineas[n_filas] = strdup(buffer); //
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

    // Leer el archivo
    char **lineas = leerArchivo(archivo);

    // Imprime primeras 3 lines para verificacion.
    for (int i = 0; i < 3; i++) {
        printf("Línea %d: %s\n", i + 1, lineas[i]);
    }

    pthread_t hebras[num_hebras];              // Array para las hebras
    data_hebras datos_para_hebras[num_hebras]; // Datos para cada hebra

    // Crear hebras
    for (int i = 0; i < num_hebras; i++) {
        datos_para_hebras[i].lineas = lineas;
        datos_para_hebras[i].id_hebra = i;
        datos_para_hebras[i].inicio = i * (n_filas / num_hebras);    // Calcular inicio
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

    // Liberar memoria
    for (int i = 0; i < n_filas; i++)
    {
        free(lineas[i]);
    }
    free(lineas);
    printf("fin programa\n");
    return 0;
}