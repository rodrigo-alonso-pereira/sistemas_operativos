#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Nombre del archivo de texto
    const char *nombreArchivo = "archivo.txt";
    
    // Abre el archivo en modo lectura
    FILE *archivo = fopen(nombreArchivo, "r");
    
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }
    
    char linea[1000]; // Tamaño máximo de la línea (ajusta según tus necesidades)
    
    // Lee una línea del archivo
    if (fgets(linea, sizeof(linea), archivo) != NULL) {
        // Usamos strtok para dividir la línea en palabras
        char *palabra = strtok(linea, " "); // Dividir en base a espacios en blanco
        
        while (palabra != NULL) {
            printf("%s\n", palabra);
            palabra = strtok(NULL, " "); // Avanzar a la siguiente palabra
        }
    }
    
    // Cierra el archivo
    fclose(archivo);
    
    return 0;
}

