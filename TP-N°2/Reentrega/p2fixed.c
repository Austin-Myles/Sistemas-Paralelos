//Algoritmo paralelo empleando Pthreads
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<pthread.h>

//Creamos las matrices
double *A,*B,*C,*D,*AxB,*CxD,*R;

//Creamos las variables globales
int N, bs, T;

//Creamos las variables maximas, minimas y promedios y les declaramos valores.
double maxA = -1;
double maxB = -1;
double minA = 10000; 
double minB = 10000; 
double promA = 0; 
double promB = 0;

//Creamos los mutex_lock para actualizar las variables max y min.
pthread_mutex_t mutexT1 = PTHREAD_MUTEX_INITIALIZER;

//Creamos la barrier para actualizar los promedios.
pthread_barrier_t barrier;


//Tiempo en segundos desde un momento pasado
double dwalltime();

//Multiplica submatrices del recorrido por bloques

void *multiply_thread(void *arg)
{
    //Creamos los indices y auxiliares
    int i, j, k, i2, j2, k2, id, inicio, fin;
    int auxI, auxJ, auxK, auxI2, auxJ2, auxK2;

    //Creamos los auxiliares para los maximos y minimos.
    double localMaxA = -1;
    double localMaxB = -1;
    double localMinA = 10000;
    double localMinB = 10000;
    double localPromA = 0;
    double localPromB = 0;
    double T1 = 0;


    id = *((int *) arg);
    inicio = (N*id)/T;
    fin = (N*(id + 1))/T;

    //Obtenemos los maximos, minimos y promedios
    for(i = inicio; i<fin; i++){
        auxI = i * N;
        for(j=0; j<N; j++){
            if(A[auxI + j] > localMaxA){
                localMaxA = A[auxI + j];
            }
            if(A[auxI + j] < localMinA){
                localMinA = A[auxI + j];
            }
            localPromA += A[auxI + j];
        }
    }
    for(i= inicio; i<fin; i++){
        auxI = i * N;
        for(j=0; j<N; j++){
            if(B[auxI + j] > localMaxB){
                localMaxB = B[auxI + j];
            }
            if(B[auxI + j] < localMinB){
                localMinB = B[auxI + j];
            }
            localPromB += B[auxI + j];
        }
    }

    pthread_mutex_lock(&mutexT1);
    if(localMaxA > maxA){
        maxA = localMaxA;
    }
    if(localMinA < minA){
            minA = localMinA;
    }
    if(localMaxB > maxB){
        maxB = localMaxB;
    }
    if(localMinB < minB){
        minB = localMinB;
    }
    promA += localPromA;
    promB += localPromB;
    pthread_mutex_unlock(&mutexT1);
    
    //Sumamos los promedios
    pthread_barrier_wait(&barrier);
    
    //Una vez finalizada la espera el proceso 0 calcula el primer termino
    if(id == 0){
        promA = promA / (N*N);
        promB = promB / (N*N);
        T1 = ((maxA * maxB - minA * minB) / (promA * promB));
    }

    pthread_barrier_wait(&barrier);

    //Realizamos la multiplicacion de Matrices
    for(i= inicio; i < fin; i+=bs){ 
        auxI = i * N;
        for(j=0; j < N; j+=bs){
            auxJ = j * N;
            for(k=0; k < N; k+=bs){
                for (i2 = 0; i2 < bs; i2++){
                    auxI2 = i2 * N;
                    for (j2 = 0; j2 < bs; j2++){
                        auxJ2 = j2 * N;
                        for (k2 = 0; k2 < bs; k2++){
                            AxB[auxI + j + auxI2 + j2] += A[auxI + k + auxI2 + k2] * B[auxJ + k + auxJ2 + k2];
    }}}}}}

    for(i= inicio; i < fin; i+=bs){
        auxI = i * N;
        for(j=0; j < N; j+=bs){
            auxJ = j * N;
            for(k=0; k < N; k+=bs){
                for (i2 = 0; i2 < bs; i2++){
                    auxI2 = i2 * N;
                    for (j2 = 0; j2 < bs; j2++){
                        auxJ2 = j2 * N;
                        for (k2 = 0; k2 < bs; k2++){
                            CxD[auxI + j + auxI2 + j2] += C[auxI + k + auxI2 + k2] * D[auxJ + k + auxJ2 + k2];
    }}}}}}

    //Cada proceso realiza la operaci�n por bloques de la matriz R

    for(i=inicio ; i < fin; i++){
        auxI = i * N;
        for(j=0; j < N; j++){
            R[auxI + j] += (AxB[auxI + j] * T1) + CxD[auxI + j];
        }
    } 

    pthread_exit(NULL);  
}

int main(int argc,char*argv[]){

    int i, j;

    double timetick;

    /* Check command line parameters */
    if ( (argc != 4) || ((N = atoi(argv[2])) <= 0) || ((bs = atoi(argv[3])) <= 0) || ((N % bs) != 0))
    {
        printf("\nError en los parametros. Usage: ./%s T N BS (N debe ser multiplo de BS)\n", argv[0]);
        exit(1);
    }
    
    double T1;

    T = atoi(argv[1]);
    //Declaramos los hilos y el vector de argumentos
    pthread_t threads[T];

    int ids[T];

    //Declaramos los mutex_lock para actualizar las variables max y min.
    pthread_mutex_init(&mutexT1, NULL);

    //Declaramos la barrier para actualizar los promedios.
    pthread_barrier_init(&barrier, NULL, T);


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

    //Hacemos las multiplicaciones de matrices, por bloques (A y C por filas; B y D por columnas)
    for(i=0; i < T; i++){
        //Hacemos los cambios en la matriz resultado CxD
        ids[i] = i;
        pthread_create(&threads[i], NULL, multiply_thread, &ids[i]);
    }

    // Esperamos a que los hilos terminen.
    for(int i = 0; i < T; i++){
        pthread_join(threads[i], NULL);
    }

    //Fin de paralelizacion

    double time = dwalltime() - timetick;

    printf("Ecuacion con dimension NxN %d con block size %d y %d threads. Tiempo en segundos %f\n",N,bs,T,time);

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(AxB);
    free(CxD);

    //Destruimos el mutex_lock
    pthread_mutex_destroy(&mutexT1);
    
    //Destruimos la barrier
    pthread_barrier_destroy(&barrier);

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

/**************************/