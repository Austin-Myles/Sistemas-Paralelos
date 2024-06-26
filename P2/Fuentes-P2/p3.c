
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

//Ocurrencias y Vector
int *A;
int ocurrencia = 0;

//Dimension por defecto del vector, el numero a encontrar y el numero de hilos
int N;
int X = 0; 
int H = MAX_THREADS;

//Mutex
pthread_mutex_t mutex;

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

//Moduelo de busqueda

void *buscar_elemento(void *arg){
    
    ThreadArgs *args = (ThreadArgs *)arg;
    int inicio = args->inicio;
    int fin = args->fin;
    int id_hilo = args->id_hilo;

    for(int i = inicio; i < fin; i++){
        if(A[i] == X ){
            pthread_mutex_lock(&mutex);
            ocurrencia++;
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    double timetick;
    int check=0;

    //Creamos el Mutex
    pthread_mutex_init(&mutex, NULL);

    //Controla los argumentos al programa
    if ((argc != 4) || ((N = atoi(argv[1])) <= 0) || ((X = atoi(argv[2])) < 0) || ((H = atoi(argv[3])) <= 0)) {
        printf("\nUsar: %s N X H\n N: Tamaño del vector\n X: Elemento a buscar\n H: Cantidad de hilos\n", argv[0]);
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
        pthread_create(&threads[i], NULL, buscar_elemento, (void *)&args[i]);
    }

    // Esperamos a que los hilos terminen.
    for(int i = 0; i < H; i++){
        pthread_join(threads[i], NULL);

    }

    printf("Busqueda de elemento %d en el vector tamaño %d. Tiempo en segundos %f\n", X, N, dwalltime() - timetick);

    printf("Cantidad de ocurrencias: %d",ocurrencia);
    printf("\n");

    pthread_mutex_destroy(&mutex);
    free(A);
    return 0;
}
