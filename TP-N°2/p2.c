//Algoritmo paralelo empleando Pthreads

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<pthread.h>


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
    double *A;
    double *B;
    double *AxB;
    double *C;
    double *D;
    double *CxD;
    
} ThreadArgs;

//Multiplica submatrices del recorrido por bloques

void *thread_job(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;

    int i, j, k;

    for (i = args->inicio; i < args->fin; i += args->bs)
    {
        for (j = 0; j < args->N; j += args->bs)
        {
            for  (k = 0; k < args->N; k += args->bs)
            {
                blkmul(&args->A[i*args->N + k], &args->B[j*args->N + k], &args->AxB[i*args->N + j], args->N, args->bs);
                blkmul(&args->C[i*args->N + k], &args->D[j*args->N + k], &args->CxD[i*args->N + j], args->N, args->bs);
            }
        }
    }
    pthread_exit(NULL);  
}


int main(int argc,char*argv[]){

    int N, bs, i, j, numThreads;

    double timetick;

    /* Check command line parameters */
    if ( (argc != 3) || ((N = atoi(argv[1])) <= 0) || ((numThreads = atoi(argv[2])) <= 0) || ((N % numThreads) != 0) )
    {
        printf("\nError en los par�metros. Usage: ./%s N T (N debe ser multiplo de T)\n", argv[0]);
        exit(1);
    }
    
    //Creamos las matrices
    double *A,*B,*C,*D,*AxB,*CxD,*R;

    //Creamos las variables maximas, minimas y promedios y les declaramos valores.
    double maxA = -1;
    double maxB = -1;
    double minA = 10000; 
    double minB = 10000; 
    double promA = 0; 
    double promB = 0;
    
    double T1;

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
    
    //Dividimos por bloques las matrices y la designamos a los hilos
    bs = N / numThreads;

    //Hacemos las multiplicaciones de matrices, por bloques (A y C por filas; B y D por columnas)
    for(i=0; i < numThreads; i++){
        //Hacemos los cambios en la matriz resultado CxD
        args[i].inicio = i * bs;
        args[i].fin = (i + 1) * bs;
        args[i].N = N;
        args[i].bs = bs;
        args[i].A = A;
        args[i].B = B;
        args[i].AxB = AxB;
        args[i].C = C;
        args[i].D = D;
        args[i].CxD = CxD;

        pthread_create(&threads[i], NULL, thread_job, &args[i]);
    }

    //Multiplicamos T1 por cada valor de la matriz AxB
    for(i=0; i < N*N; i++){
		//Aprovechamos y hacemos la suma de matrices en el mismo recorrido
        R[i] = (AxB[i] * T1) + CxD[i]; 
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

void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs)
{
  int i, j, k;

  for (i = 0; i < bs; i++)
  {
    for (j = 0; j < bs; j++)
    {
      for  (k = 0; k < bs; k++)
      {
        cblk[i*n + j] += ablk[i*n + k] * bblk[j*n + k];
      }
    }
  }
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