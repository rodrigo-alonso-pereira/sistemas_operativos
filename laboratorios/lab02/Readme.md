# Laboratorio 2: Análisis Paralelo de Dataset Netflix con Hebras

- Profesor: Gabriel Godoy
- Ayudante: Reinaldo Pacheco
- Alumno: Rodrigo Pereira
- Asignatura: Sistemas Operativos, vespertino 2025
- Fecha: 1 de noviembre de 2025

## Descripción

Este programa analiza un dataset de títulos de Netflix (`netflix_titles.csv`) para contar la frecuencia de contenido por país de origen, utilizando **procesamiento paralelo con hebras (threads)** y sincronización mediante mutex.

## ¿Cómo funciona?

El programa divide el trabajo de análisis entre **N hebras configurables**, donde cada hebra:

1. **Lee un rango específico de líneas** del archivo CSV
2. **Extrae el campo de país** (columna 5) de cada título
3. **Procesa países con formato especial**: Si el campo contiene múltiples países entre comillas (ej: `"United States, Canada"`), toma solo el primero
4. **Actualiza una lista compartida** de países usando mutex para evitar condiciones de carrera
5. **Ordena los resultados** por frecuencia descendente usando `qsort`

### Arquitectura de hebras

Si se especifican **3 hebras** y hay 8000 líneas:
- **Hebra 0**: Procesa líneas 0 - 2666
- **Hebra 1**: Procesa líneas 2667 - 5333
- **Hebra 2**: Procesa líneas 5334 - 8000

## Compilación

```bash
gcc lab2_paralizacion_hebras_16610470k.c -o lab2 -pthread
```

## Uso

```bash
./lab2 <archivo_csv> <numero_hebras>
```

### Ejemplos

```bash
# Usar 1 hebra (ejecución secuencial)
./lab2 netflix_titles.csv 1

# Usar 4 hebras (ejecución paralela)
./lab2 netflix_titles.csv 4

# Usar 8 hebras (máximo paralelismo)
./lab2 netflix_titles.csv 8
```

## Formato de entrada

El programa espera un archivo CSV con formato Netflix:

```csv
show_id,type,title,director,cast,country,date_added,release_year,rating,duration,listed_in,description
s1,Movie,Dick Johnson Is Dead,Kirsten Johnson,,United States,"September 25, 2021",2020,PG-13,90 min,Documentaries,"As her father..."
s2,TV Show,Blood & Water,,"Ama Qamata, Khosi Ngema","South Africa, USA","September 24, 2021",2021,TV-MA,2 Seasons,International TV Shows,"After crossing..."
```

**Campos importantes:**
- **Columna 5 (country)**: País de origen del contenido
- Puede contener múltiples países: `"United States, Canada, Mexico"`
- El programa solo cuenta el **primer país** de la lista

## Salida esperada

### Por pantalla:
```
Se leyeron 8807 líneas del archivo
Hebra 0: Procesando paises del 0 al 2935
Hebra 1: Procesando paises del 2936 al 5871
Hebra 2: Procesando paises del 5872 al 8807
Main: Hebra 0 ha terminado.
Main: Hebra 1 ha terminado.
Main: Hebra 2 ha terminado.
Reporte generado: reporte_paises_netflix.txt

...
fin programa
```

### Archivo de salida:

El programa genera automáticamente un archivo **`reporte_paises_netflix.txt`** con los resultados ordenados de **mayor a menor** frecuencia:

```
Pais Principal          | Titulos
---------------------------------
United States           | 3211
India                   | 1008
United Kingdom          | 628
Canada                  | 271
Japan                   | 259
France                  | 212
...
```

Este archivo está estructurado con:
- **Encabezado** con nombres de columnas
- **Separador visual** (línea de guiones)
- **Datos alineados** en formato tabla
- **Orden descendente** por número de títulos (país con más contenido primero)

## Características técnicas

- **Lenguaje**: C (C99)
- **Paralelismo**: Hebras POSIX (pthreads)
- **Sincronización**: Mutex (`pthread_mutex_t`) para proteger estructuras compartidas
- **Memoria dinámica**: Arrays redimensionables con `realloc`
- **Ordenamiento**: `qsort` con comparador personalizado
- **Parsing CSV**: Manejo de campos con comillas y comas
- **Salida**: Consola + archivo de texto formateado

## Archivos incluidos

- `lab2_paralizacion_hebras_16610470k.c` - Código fuente principal
- `netflix_titles.csv` - Archivo .csv con informacion de peliculas netflix
- `Readme.md` - Este archivo de documentación

## Notas
El archivo `netflix_titles.csv` puede contener saltos de lineas que generan problemas con el conteo, ya que el patron de busqueda solicitado por este laboratorio fue con base a un formato indicado en head del archivo, si no se cumple, puede generar un conteo erroneo.

---
Código 👽 por rodrigo-alonso-pereira
