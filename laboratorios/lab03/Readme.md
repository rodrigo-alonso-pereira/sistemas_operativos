# Laboratorio 3: Procesamiento Paralelo de Ventas Cyber Day con Hebras

- Profesor: Gabriel Godoy
- Ayudante: Reinaldo Pacheco
- Alumno: Rodrigo Pereira
- Asignatura: Sistemas Operativos, vespertino 2025
- Fecha: 16 de noviembre de 2025

## Descripción

Este programa procesa un archivo de texto con ventas del Cyber Day 2025 (`ventas_cyberday_2025.txt`) para contar la cantidad de unidades vendidas por producto, utilizando **procesamiento paralelo con hebras (threads)**, sincronización mediante mutex y barreras, y estructuras de datos de **listas enlazadas**.

## ¿Cómo funciona?

El programa divide el trabajo de análisis entre **N hebras configurables**, donde cada hebra:

1. **Lee el archivo** línea por línea y construye una lista enlazada con los nombres de productos
2. **Procesa un rango específico** de la lista enlazada según su ID de hebra
3. **Actualiza una lista compartida** (lista resumen) que cuenta las unidades vendidas por producto usando mutex para evitar condiciones de carrera
4. **Se sincroniza** con las demás hebras usando barreras antes de finalizar
5. **Genera un archivo de salida** con los resultados del conteo
6. **Permite consultas interactivas** por consola para buscar productos específicos

### Arquitectura de hebras

Si se especifican **3 hebras** y hay 1000 productos:
- **Hebra 0**: Procesa productos 0 - 333
- **Hebra 1**: Procesa productos 334 - 666
- **Hebra 2**: Procesa productos 667 - 1000

## Compilación

```bash
gcc lab3_procesamiento_paralelo_16610470k.c -o lab3 -pthread
```

## Uso

```bash
./lab3 <archivo_txt> <numero_hebras>
```

### Ejemplos

```bash
# Usar 1 hebra (ejecución secuencial)
./lab3 ventas_cyberday_2025.txt 1

# Usar 4 hebras (ejecución paralela)
./lab3 ventas_cyberday_2025.txt 4

# Usar 8 hebras (máximo paralelismo)
./lab3 ventas_cyberday_2025.txt 8
```

## Formato de entrada

El programa espera un archivo de texto simple donde cada línea contiene el nombre de un producto vendido:

```
Iphone 17 Pro Max
Smart TV 55 Pulgadas
Alexa Echo Dot 5ta generacion
Playstation 5
Iphone 17 Pro Max
Smart TV 55 Pulgadas
Iphone 17 Pro Max
...
```

**Características del archivo:**
- Una línea por producto vendido
- Sin formato especial (no es CSV)
- Productos repetidos representan múltiples ventas

## Salida esperada

### Por pantalla:
```
Hebra 0: Procesando paises del 0 al 333
Hebra 1: Procesando paises del 334 al 666
Hebra 2: Procesando paises del 667 al 999
Main: Hebra 0 ha terminado.
Main: Hebra 1 ha terminado.
Main: Hebra 2 ha terminado.
Reporte generado: reporte_ventas_final.txt

--------------------------------------------------------------
Consulta de ventas por producto (escriba 'salir' para terminar):
--------------------------------------------------------------
Ingrese el nombre del producto para ver sus unidades vendidas: Iphone 17 Pro Max
El producto 'Iphone 17 Pro Max' vendió 485 unidades.
Ingrese el nombre del producto para ver sus unidades vendidas: Playstation 5
El producto 'Playstation 5' vendió 198 unidades.
Ingrese el nombre del producto para ver sus unidades vendidas: salir

...
fin programa
```

### Archivo de salida:

El programa genera automáticamente un archivo **`reporte_ventas_final.txt`** con los resultados del conteo:

```
Resultado Ventas Cyber Day - 2025:

Iphone 17 Pro Max: 485 unidades
Smart TV 55 Pulgadas: 185 unidades
Playstation 5: 198 unidades
Alexa Echo Dot 5ta generacion: 132 unidades
```

**Características del archivo:**
- Título descriptivo del reporte
- Lista de productos con sus unidades vendidas
- Formato: `Nombre Producto: X unidades`
- El orden depende del orden de inserción en la lista (último procesado primero)

## Características técnicas

- **Lenguaje**: C (C99)
- **Paralelismo**: Hebras POSIX (pthreads)
- **Sincronización**: Mutex (`pthread_mutex_t`) y barreras (`pthread_barrier_t`)
- **Estructura de datos**: Listas enlazadas simples con memoria dinámica
- **Entrada/Salida**: Lectura con `getline()`, escritura con `fprintf()`
- **Consulta interactiva**: Búsqueda de productos por consola con `fgets()`
- **Salida**: Consola + archivo de texto (`reporte_ventas_final.txt`)

## Archivos incluidos

- `lab3_procesamiento_paralelo_16610470k.c` - Código fuente principal
- `ventas_cyberday_2025.txt` - Archivo de texto con ventas (1000+ líneas)
- `reporte_ventas_final.txt` - Archivo de salida generado por el programa
- `Readme.md` - Este archivo de documentación

## Funcionalidades adicionales

### Generación de reporte
El programa crea automáticamente un archivo de salida con los resultados del procesamiento. El archivo incluye:
- Título del reporte
- Lista completa de productos y sus unidades vendidas
- Formato legible y simple

### Consulta interactiva
Después de procesar los datos, el programa permite realizar consultas por consola:
- El usuario puede ingresar el nombre exacto de un producto
- El programa busca en la lista resumen y muestra las unidades vendidas
- Para salir de la consulta, escribir `salir`
- Útil para verificar productos específicos sin revisar todo el archivo

**Ejemplo de uso:**
```
Ingrese el nombre del producto para ver sus unidades vendidas: Smart TV 55 Pulgadas
El producto 'Smart TV 55 Pulgadas' vendió 185 unidades.
```

## Notas

### Validación de entrada
En la función `leer_archivo` se implementaron validaciones adicionales para garantizar un conteo correcto:

1. **Eliminación de BOM UTF-8**: Se detecta y elimina el BOM (Byte Order Mark) `\xEF\xBB\xBF` al inicio del archivo que puede causar que el primer producto no se cuente correctamente.

2. **Limpieza de saltos de línea**: Se eliminan caracteres `\n` y `\r` al final de cada línea para evitar duplicados.

3. **Eliminación de espacios**: Se remueven espacios en blanco al inicio y final de cada nombre de producto usando `isspace()` para asegurar comparaciones correctas con `strcmp`.

4. **Validación de líneas vacías**: Se ignoran líneas vacías o que solo contienen espacios en blanco.

Estas validaciones son críticas para el correcto funcionamiento del programa, especialmente cuando el archivo proviene de diferentes sistemas operativos (Windows, Linux, macOS) o editores de texto que pueden agregar caracteres invisibles.

### Estructura de listas enlazadas
El programa utiliza dos listas enlazadas separadas:

1. **`lista_nombre_productos`**: Lista con todos los nombres de productos leídos del archivo (puede tener duplicados). Se usa para distribuir el trabajo entre hebras.

2. **`lista_resumen`**: Lista compartida (protegida por mutex) que contiene productos únicos con su contador de unidades vendidas. Esta es la sección crítica del programa.

La inserción en ambas listas se hace al inicio (`inicio = nuevo_nodo`), por lo que el orden en la lista es inverso al orden del archivo original.

---
Código 👽 por rodrigo-alonso-pereira
