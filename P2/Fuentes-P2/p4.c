
#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<pthread.h>
#include<sys/time.h>


#define MAX_THREADS 8

//Estructura para los argumentos de los hilos

typedef struct {
    /* data */
    int inicio;
    int fin;
    int id_hilo;
} ThreadArgs;

//Maximo, minimo, promedio y Vector
int *A;
int max = 0;
int min = 10000;
double avg = 0;

//Dimension por defecto del vector y el numero de hilos
int N;
int H = MAX_THREADS;

//Definición del semaforo
pthread_mutex_t mutex;
pthread_mutex_t mutexMax;
pthread_mutex_t mutexMin;

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

//Modulo de calculo
void *calcular_elementos(void *arg){
    
    ThreadArgs *args = (ThreadArgs *)arg;
    int inicio = args->inicio;
    int fin = args->fin;
    int id_hilo = args->id_hilo;

    for(int i = inicio; i < fin; i++){
        if(A[i] > max){
            pthread_mutex_lock(&mutexMax);
            max = A[i];
            pthread_mutex_unlock(&mutexMax);
        }
        if(A[i] < min){
            pthread_mutex_lock(&mutexMin);
            min = A[i];
            pthread_mutex_unlock(&mutexMin);
        }
        pthread_mutex_lock(&mutex);
        avg += A[i];
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    double timetick;
    int check=0;

    //Creamos el Mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexMax, NULL);
    pthread_mutex_init(&mutexMin, NULL);

    //Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((H = atoi(argv[2])) < 0)) {
        printf("\nUsar: %s N H\n N: Tamaño del vector\n H: Cantidad de hilos\n", argv[0]);
        exit(1);
    }

    //Aloca memoria para el vector
    A = (int *)malloc(N * sizeof(int));

    //Inicializa el vector A
    srand(time(NULL));   // Para generar numeros random..
    for(int i = 0; i < N; i++){
        A[i] = rand() % 100;
    }

    //Realiza la multiplicacion
    timetick = dwalltime();

    //Dividimos por bloques la matriz y la designamos a los Threads
    //Creamos los threads
    pthread_t threads[H];
    ThreadArgs args[H];
    
    int bloque_size = N / H;

    for(int i = 0; i < H; i++) {
        args[i].inicio = i * bloque_size;
        args[i].fin = (i < H - 1) ? (i + 1) * bloque_size : N;
        args[i].id_hilo = i;
        pthread_create(&threads[i], NULL, calcular_elementos, (void *)&args[i]);
    }

    // Esperamos a que los hilos terminen.
    for(int i = 0; i < H; i++){
        pthread_join(threads[i], NULL);
    }

    // Calculamos finalmente avg
    avg = avg / N;

    printf("Calculo en el vector tamaño %d. Tiempo en segundos %f\n", N, dwalltime() - timetick);

    printf("Max: %d.\nMin: %d.\nPromedio: %f.\n",max,min,avg);
    printf("\n");

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexMax);
    pthread_mutex_destroy(&mutexMin);
    free(A);
    return 0;
}
