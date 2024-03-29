//𝐴𝑖 = 𝐵𝑖 + 𝐶𝑖. usando paralalelismo mediante hilos...

#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<pthread.h>
#include<sys/time.h>

#define MAX_THREADS 8

int T=MAX_THREADS ;


/* Multiply square matrices, blocked version */
void sumavectorial(double *a, double *b, double *c, int n, int t);

int main(int argc,char*argv[]){
    double *A,*B,*C;
    int i,j,k,N,T;

    double timetick;
    int  ids[MAX_THREADS];
    pthread_attr_t attr;
    pthread_t threads[MAX_THREADS];
	int *status;

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0) )
  {
    printf("\nUsar: %s T\n  N: tamaño de los vectores \n  T: cantidad de hilos\n", argv[0]);
    exit(1);
  }

 	pthread_attr_init(&attr); 

    /* Crea los hilos */
    for (i = 0; i < T; i++) {
	    ids[i] = i;
        pthread_create(&threads[i], &attr, hello_word, &ids[i]);
    }

    /* Espera a que los hilos terminen */
    for (i = 0; i < T; i++) {
        pthread_join(threads[i], (void*) &status);
		printf("\n Hilo %d retornó %d",i,(*status));
	}

 return(0);
}


void * sumavectorial (double *a, double *b, double *c, int n, int t) {
	
	int * p, id;
    p = (int *) ptr;
    id = *p;

	printf("\nHola mundo! Soy el hilo %d.",id);
	
    pthread_exit((void*)ptr);
}