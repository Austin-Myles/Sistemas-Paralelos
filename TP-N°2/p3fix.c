//Algoritmo paralelo empleando OpenMP

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<omp.h>

#define OMP_NUM_THREADS 4

//Multiplica submatrices del recorrido por bloques
void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

//Tiempo en segundos desde un momento pasado
double dwalltime();

int main(int argc,char*argv[]){

    int N, bs, i, j, k, numThreads, i2, j2, k2;

    double timetick;

    /* Check command line parameters */
    if ( (argc != 4) || ((N = atoi(argv[1])) <= 0) || ((bs = atoi(argv[2])) <= 0) || ((N % bs) != 0))
    {
        printf("\nError en los par�metros. Usage: ./%s N BS (N debe ser multiplo de BS)\n", argv[0]);
        exit(1);
    }
    //Creamos las matrices
    double *A,*B,*C,*D,*AxB,*CxD,*ablk,*bblk,*cblk,*R;

    //Aplicamos los Threads
    numThreads = atoi(argv[3]);
    omp_set_num_threads(numThreads);

    //Creamos las variables maximas, minimas y promedios y les declaramos valores.
    double maxA = -1;
    double maxB = -1;
    double minA = 10000; 
    double minB = 10000; 
    double promA = 0; 
    double promB = 0;
    
    double T1;

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

    #pragma omp parallel for collapse(3) shared(C, D, CxD) private(i, j, k, i2, j2, k2)
    for(i=0; i < N; i+=bs){
        for(j=0; j < N; j+=bs){
            for(k=0; k < N; k+=bs){
                //Hacemos la multiplicacion de bloques
                for (i2 = 0; i2 < bs; i2++)
                {
                    for (j2 = 0; j2 < bs; j2++)
                    {
                        for  (k2 = 0; k2 < bs; k2++)
                        {
                            CxD[(i+i2)*N + (j+j2)] += C[(i+i2)*N + (k+k2)] * D[(j+j2)*N + (k+k2)];
                        }
                    }
                }
            }
        }
    }
    
    


    #pragma omp parallel for collapse(3) shared(A, B, AxB) private(i, j, k, i2, j2, k2)
    for(i=0; i < N; i+=bs){
        for(j=0; j < N; j+=bs){
            for(k=0; k < N; k+=bs){
                //Hacemos la multiplicacion de bloques
                for (i2 = 0; i2 < bs; i2++)
                {
                    for (j2 = 0; j2 < bs; j2++)
                    {
                        for  (k2 = 0; k2 < bs; k2++)
                        {
                            AxB[(i+i2)*N + (j+j2)] += A[(i+i2)*N + (k+k2)] * B[(j+j2)*N + (k+ k2)];
                        }
                    }
                }
            }
        }
    }
    
    
    //Multiplicamos T1 por cada valor de la matriz AxB
    #pragma omp parallel for shared(R, AxB, CxD, T1) private(i)
    for(i=0; i < N*N; i++){
        //Aprovechamos y hacemos la suma de matrices en el mismo recorrido
        R[i] = (AxB[i] * T1) + CxD[i]; 
    }
    


    double time = dwalltime() - timetick;

    printf("Ecuacion con dimension NxN %d con block size %d. Cantidad de Threads %d . Tiempo en segundos %f\n",N,bs,numThreads,time);

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
