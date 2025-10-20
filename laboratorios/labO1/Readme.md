# Laboratorio 1: Conteo Paralelo de Dígitos en un Archivo

- Profesor: Gabriel Godoy
- Ayudante: Reinaldo Pacheco
- Alumno: Rodrigo Pereira
- Asignatura: Sistemas Operativos, vespertino 2025
- Fecha: 27 de septiembre de 2025

## Descripción

Este programa cuenta la frecuencia de dígitos (0-9) en un archivo de texto utilizando **procesamiento paralelo** con procesos hijo y comunicación mediante pipes.

## ¿Cómo funciona?

El programa divide el trabajo de conteo en **5 procesos hijo**, cada uno responsable de contar un par de dígitos:

- **Proceso 1**: Cuenta dígitos 0 y 1
- **Proceso 2**: Cuenta dígitos 2 y 3  
- **Proceso 3**: Cuenta dígitos 4 y 5
- **Proceso 4**: Cuenta dígitos 6 y 7
- **Proceso 5**: Cuenta dígitos 8 y 9

Cada proceso hijo lee el archivo completo, cuenta solo sus dígitos asignados, y envía el resultado al proceso padre mediante un pipe. El proceso padre espera a que todos los hijos terminen y luego recolecta y suma los resultados.

## Compilación

```bash
gcc lab1_conteo_paralelo_16610470k.c -o conteo_paralelo
```

## Uso

```bash
./conteo_paralelo <nombre_archivo>
```

### Ejemplo

```bash
./conteo_paralelo numeros.txt
```

## Formato de entrada

El programa puede procesar cualquier archivo de texto que contenga dígitos. Ejemplos:

**numeros.txt:**
```
12345
67890
asdfa123asdfasd

asdfasdf 1 adsfasdf

1
```

## Salida esperada

```
Conteo de digitos: 
0-1: 5
2-3: 4
4-5: 2
6-7: 2
8-9: 2
Total Digitos: 15
```

## Características técnicas

- **Lenguaje**: C
- **Paralelismo**: 5 procesos hijo concurrentes
- **Comunicación**: Pipes unidireccionales
- **Sincronización**: wait() para esperar terminación de procesos
- **Lectura**: Carácter por carácter con fgetc()

## Archivos incluidos

- `lab1_conteo_paralelo_16610470k.c` - Código fuente principal
- `numeros.txt` - Archivo de prueba con números
- `Readme.md` - Este archivo de documentación

## Notas

- El programa ignora caracteres que no sean dígitos (letras, espacios, símbolos)
- Cada proceso lee el archivo completo pero solo cuenta sus dígitos asignados
- La ejecución es paralela real: los 5 procesos trabajan simultáneamente

---
Código 👽 por rodrigo-alonso-pereira
