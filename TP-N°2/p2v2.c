//Algoritmo paralelo empleando Pthreads

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<pthread.h>

//Creamos las matrices
double *A,*B,*C,*D,*AxB,*CxD,*R, T1;

//Multiplica submatrices del recorrido por bloques
void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

//Tiempo en segundos desde un momento pasado
double dwalltime();

//Definición de la estructura para los argumentos de los threads
typedef struct {
    //Datos del Thread 
    int inicio;
    int fin;
    int bs;
    int N;
    int id;
    
} ThreadArgs;

//Multiplica submatrices del recorrido por bloques

void *multiply_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;

    int i2, j2, k2;

    //Hacemos la multiplicacion de bloques
    for (i2 = 0; i2 < args->bs; i2++)
    {
    for (j2 = 0; j2 < args->bs; j2++)
    {
    for  (k2 = 0; k2 < args->bs; k2++)
    {
        CxD[i2*args->N + j2] += C[i2*args->N + k2] * D[j2*args->N + k2];
    }
    }
    }


    //Hacemos la multiplicacion de bloques
    for (i2 = 0; i2 < args->bs; i2++)
    {
    for (j2 = 0; j2 < args->bs; j2++)
    {
    for  (k2 = 0; k2 < args->bs; k2++)
    {
        AxB[i2*args->N + j2] += A[i2*args->N + k2] * B[j2*args->N + k2];
    }
    }
    }

    //Multiplicamos T1 por cada valor de la matriz AxB
    for(int i=0; i < args->N*args->N; i++){
		//Aprovechamos y hacemos la suma de matrices en el mismo recorrido
        R[i] = (AxB[i] * T1) + CxD[i]; 
    }
    //printf("Termino el Hilo %d \n", args->id);
    pthread_exit(NULL);  
}


int main(int argc,char*argv[]){

    int N, bs, i, j, numThreads;

    double timetick;

    /* Check command line parameters */
    if ( (argc != 4) || ((N = atoi(argv[2])) <= 0) || ((bs = atoi(argv[3])) <= 0) || ((N % bs) != 0))
    {
        printf("\nError en los par�metros. Usage: ./%s T N BS (N debe ser multiplo de BS)\n", argv[0]);
        exit(1);
    }

    numThreads = atoi(argv[1]);
    
    //Creamos las variables maximas, minimas y promedios y les declaramos valores.
    double maxA = -1;
    double maxB = -1;
    double minA = 10000; 
    double minB = 10000; 
    double promA = 0; 
    double promB = 0;

    //Declaramos los hilos y el vector de argumentos
    pthread_t threads[numThreads];
    ThreadArgs args[numThreads];


    //Aloca memoria para los matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);
    R=(double*)malloc(sizeof(double)*N*N);
    AxB=(double*)malloc(sizeof(double)*N*N);
    CxD=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices en 1 y R en 0
    for(i=0;i< N ;i++){
		    for(j=0; j < N; j++){
                A[i*N + j] = 1;
                B[j*N + i] = 1;
                C[i*N + j] = 1;
                D[j*N + i] = 1;
                R[i*N + j] = 0;
                AxB[i*N + j] = 0;
                CxD[i*N + j] = 0;
        }
    }

    timetick = dwalltime();

    //Obtenemos el max, min y promedio de A y B
    for(i=0; i<N*N; i++){
        if(A[i] > maxA){
            maxA = A[i];
        }
        if(A[i] < minA){
            minA = A[i];
        }
        promA += A[i];
		}
		for(i=0; i<N*N; i++){
        if(B[i] > maxB){
            maxB = B[i];
        }
        if(B[i] < minB){
            minB = B[i];
        }
        promB += B[i];
    }
    promA = promA / N*N;
    promB = promB / N*N;
    
    //Calculamos el primer termino 
    T1 = ((maxA * maxB - minA * minB) / (promA * promB));
    

    //Hacemos las multiplicaciones de matrices, por bloques (A y C por filas; B y D por columnas)
    for(i=0; i < numThreads; i++){
        //Hacemos los cambios en la matriz resultado CxD
        args[i].inicio = i * bs;
        if(((i * bs) + bs) > N){
            args[i].fin = N;
        }else{  
            args[i].fin = (i * bs) + bs;
        }
        args[i].N = N;
        args[i].bs = bs;
        args[i].id = i;
        pthread_create(&threads[i], NULL, multiply_thread, &args[i]);
    }

    // Esperamos a que los hilos terminen.
    for(int i = 0; i < numThreads; i++){
        pthread_join(threads[i], NULL);
    }

    double time = dwalltime() - timetick;

    printf("Ecuacion con dimension NxN %d con block size %d. Tiempo en segundos %f\n",N,bs,time);

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(AxB);
    free(CxD);
    return(0);
}

/*****************************************************************/

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

/*****************************************************************/