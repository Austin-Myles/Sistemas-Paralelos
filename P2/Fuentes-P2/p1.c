//𝐴𝑖 = 𝐵𝑖 + 𝐶𝑖. usando paralalelismo mediante hilos...

#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<pthread.h>
#include<sys/time.h>

#define MAX_THREADS 8

struct thread_args {
    int inicio;
    int fin;
    int thread_id;
    int *vector_B;
    int *vector_C;
    int *vector_A;
};

/* Suma vectorial */

void *suma_vectores_hilo(void *args) {
    struct thread_args *targs = (struct thread_args *)args;
    for (int i = targs->inicio; i < targs->fin; i++) {
        targs->vector_A[i] = targs->vector_B[i] + targs->vector_C[i];
    }
    printf("Hilo %d terminó.\n", targs->thread_id);
    pthread_exit(NULL);
}

//Tiempo en segundos desde un momento pasado
double dwalltime();

int main(int argc,char*argv[]){
    int i,j,k,N,T;

    double timetick;

 //Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0) || ((T = atoi(argv[2])) > MAX_THREADS) )
    {
        printf("\nUsar: %s T\n  N: tamaño de los vectores \n  T: cantidad de hilos\n  Cantidad maxima de Hilos 8\n", argv[0]);
        exit(1);
    }

 //Aloca memoria para los vectores
    int *vector_B = (int *)malloc(N * sizeof(int));
    int *vector_C = (int *)malloc(N * sizeof(int));
    int *vector_A = (int *)malloc(N * sizeof(int));

 //Inicializa los vectores B y C en 1, y el vector A en 0;  
    for(i=0; i<N; i++)
        vector_A[i] = 0;
        vector_B[i] = 1;
        vector_C[i] = 1;        

    //Crear Hilos
    
    struct thread_args args[MAX_THREADS];
    pthread_t threads[MAX_THREADS];
    int bloque_size = N / T;
    int inicio = 0;
    int fin;

    timetick = dwalltime();


    for (i = 0; i < T; i++) {
        fin = (i < T - 1) ? inicio + bloque_size : N;
        args[i].inicio = inicio;
        args[i].fin = fin;
        args[i].thread_id = i;
        args[i].vector_B = vector_B;
        args[i].vector_C = vector_C;
        args[i].vector_A = vector_A;
        pthread_create(&threads[i], NULL, suma_vectores_hilo, (void *)&args[i]);
        inicio = fin;
    }

    /* Espera a que los hilos terminen */
    for (i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
	}
    double time = dwalltime() - timetick;
    printf("Suma con dimensión N = %d y Threads = %d. Tiempo en segundos %f\n",N,T,time);

    //Verifica el resultado
    for (int i = 0; i < N; i++) {
        if (vector_A[i] != vector_B[i] + vector_C[i]) {
            printf("¡Error! La suma de vectores no es correcta.\n");
            free(vector_B);
            free(vector_C);
            free(vector_A);
            return 1;
        }
    }
    return 0;
}

#include <stdio.h>
#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

