
#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<pthread.h>
#include<sys/time.h>

#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1
#define MAX_THREADS 8

//Estructura para los argumentos de los hilos

typedef struct {
    /* data */
    int inicio;
    int fin;
    int id_hilo;
} ThreadArgs;


// Matrices
double *A, *B, *C;

//Dimension por defecto de las matrices
int N=100;

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

//Multiplicación de Matrices

void *multiplicacion_matriz(void *arg){
    
    ThreadArgs *args = (ThreadArgs *)arg;
    int inicio = args->inicio;
    int fin = args->fin;
    int id_hilo = args->id_hilo;

    for(int i = inicio; i < fin; i++){
        for(int j = 0; j < N; j++){
            C[i*N+j] = 0;
            for(int k = 0; k < N; k++){
                C[i*N+j] += A[i*N+k] * B[k*N+j];            
            }
        }
    }
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    double timetick;
    int check=1;
    pthread_t threads[MAX_THREADS];
    ThreadArgs args[MAX_THREADS];

    //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0)){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }

    //Aloca memoria para las matrices
    A = (double*)malloc(sizeof(double)*N*N);
    B = (double*)malloc(sizeof(double)*N*N);
    C = (double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
    for(int i = 0; i < N*N; i++){
        A[i] = 1;
        B[i] = 1;
    }

    //Realiza la multiplicacion
    timetick = dwalltime();

    //Dividimos por bloques la matriz y la designamos a los Threads

    int bloque_size = N / MAX_THREADS;

    for(int i = 0; i < MAX_THREADS; i++) {
        args[i].inicio = i * bloque_size;
        args[i].fin = (i < MAX_THREADS - 1) ? (i + 1) * bloque_size : N;
        args[i].id_hilo = i;
        pthread_create(&threads[i], NULL, multiplicacion_matriz, (void *)&args[i]);
    }

    // Esperamos a que los hilos terminen.
    for(int i = 0; i < MAX_THREADS; i++){
        pthread_join(threads[i], NULL);

    }

    printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n", N, N, dwalltime() - timetick);

    // Verifica el resultado
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            check = check && (C[i * N + j] == N);
        }
    }
    if (check) {
        printf("Multiplicacion de matrices resultado correcto\n");
    } else {
        printf("Multiplicacion de matrices resultado erroneo\n");
    }

    free(A);
    free(B);
    free(C);
    return 0;
}
