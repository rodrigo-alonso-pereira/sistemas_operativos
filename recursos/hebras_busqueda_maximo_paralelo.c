#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 1024 // Tamaño del vector

/*

    PARA COMPILAR ESTE PROGRAMA SE DEBE LINKEAR LA LIBRERÍA PTHREAD AL COMPILAR MEDIANTE EL COMANDO -l

*/

/* La cantidad de hebras a levantar depende de la cantidad de procesarodores que tenga el computador */

// Estructura de datos necesaria para cada hebra
typedef struct str_limite {
    int tid;  // Identifica el id del thread
    int init; // Posición inicial del vector
    int end;  // Posición final del vector
} limite;

int A[N];        // Arreglo de nums random
int nthreads;    // Cantidad de hebras utilizadas
int init, end;   // Posición inicial y final del vector en la hebra
int *maxlocales; // Arreglo que guardará los máximos locales de cada parte del vector

// Función que realizará cada thread
// Retorna *void y tiene arg*void por que así funcionan los thread
void *mymax(void *arg) {
    int max;                   // max local
    int j;                     // Utilizada para los ciclos
    limite *p = (limite *)arg; // Se castea el argumento al tipo de estructura que es
    // Como p ahora es un puntero de la estructura limite, se accede con flecha (->) a cada elemento de la estructura
    max = A[p->init];          // Se asume que el primer elemento es el mayor
    for (j = p->init + 1; j < p->end; j++)
        max = (A[j] > max) ? A[j] : max; // Para cada elemento del vector, se ve si el leído es mayor que el anteriormente guardado
    
    printf("Soy la hebra %d y finalice de buscar mi maximo\n", p->tid);
    maxlocales[p->tid] = max; // Se guarda en la memoria global el maximo local

    pthread_exit(NULL); // Se termina la ejecución de la hebra
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int i;   // Usada para iterar
    int max; // Para guardar los maximos
    for (i = 0; i < N; i++)
        A[i] = (int)random() % 100000; // Se crea un vector de enteros random

    nthreads = atoi(argv[1]); // La cantidad de threads a utilizar se reciben como argumento de la función main

    pthread_t *threads = (pthread_t *)malloc(nthreads * sizeof(pthread_t)); // Se crea un arreglo de pthread, necesario para crear los thread

    limite *array = (limite *)malloc(sizeof(limite) * nthreads); // Vector de estructuras de datos (limite) necesaria para la creacion de los thread

    maxlocales = (int *)malloc(sizeof(int) * nthreads); // Se inicializa el vector dinamico, donde se guardarán los máximos locales

    // Para cada thread, se le asigna un id, un inicio y un fin del vector
    // Como se accede a la estructura (sin ser puntero) se usa puntos (.) para acceder a cada elemento de la estructura
    for (i = 0; i < nthreads; i++) {
        array[i].tid = i;
        array[i].init = i * (N / nthreads);
        array[i].end = (i + 1) * (N / nthreads);
    }

    // Se crean las hebras necesarias
    for (i = 0; i < nthreads; i++) {
        pthread_create(&(threads[i]), NULL, mymax, (void *)&(array[i])); // Se crea cada hebra, pasandole la función y la estructura de datos correspondiente
    }

    printf("esperando a las hebras\n"); 

    // Se espera que todas las hebras finalicen
    for (i = 0; i < nthreads; i++)
        pthread_join(threads[i], NULL); // Mediante join me aseguro que las hebras finalizan

    max = maxlocales[0]; // Se asume que el primer máximo local es el global

    // Se busca el máximo global entre los máximos locales
    for (i = 1; i < nthreads; i++)
        max = (maxlocales[i] > max) ? maxlocales[i] : max; // Se ve cual es el máximo global

    for (i = 0; i < nthreads; i++)
        printf("El maximo local %i %d\n", i + 1, maxlocales[i]); // Se imprimen los máximos locales

    printf("El maximo global es : %d\n", max); // Se imprime el máximo global

    exit(0);
}
