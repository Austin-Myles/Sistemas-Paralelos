//Algoritmo paralelo empleando OpenMP

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<omp.h>

//Tiempo en segundos desde un momento pasado
double dwalltime();

int main(int argc,char*argv[]){

    int N, bs, i, j, k, numThreads, i2, j2, k2, auxI, auxJ;

    double timetick;

    /* Check command line parameters */
    if ( (argc != 3) || ((N = atoi(argv[1])) <= 0) || ((bs = atoi(argv[2])) <= 0) || ((N % bs) != 0))
    {
        printf("\nError en los par�metros. Usage: ./%s N BS (N debe ser multiplo de BS)\n", argv[0]);
        exit(1);
    }
    //Creamos las matrices
    double *A,*B,*C,*D,*AxB,*CxD,*ablk,*bblk,*cblk,*R;

    char *omp_num_threads_str = getenv("OMP_NUM_THREADS");
    if(omp_num_threads_str != NULL){
	    numThreads = atoi(omp_num_threads_str);	
    }	else {
	    numThreads = 2;
    }
	
    //Aplicamos los Threads
    //numThreads = OMP_NUM_THREADS;
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
    //Declaramos la zona de ejecución paralela
    #pragma omp parallel shared(C, D, CxD, A, B, AxB, R) private(i, j, k, i2, j2, k2, auxI, auxJ) 
    {
        //Calculamos los valores para la matriz A
        #pragma omp for schedule(static) reduction(max:maxA) reduction(min:minA) reduction(+:promA)
        for(i=0; i<N*N; i++){
            if(A[i] > maxA){
                maxA = A[i];
            }
            if(A[i] < minA){
                minA = A[i];
            }
            promA += A[i];
        }
        //Calculamos los valores para la matriz B
        #pragma omp for schedule(static) reduction(max:maxB) reduction(min:minB) reduction(+:promB)
        for(i=0; i<N*N; i++){
            if(B[i] > maxB){
                maxB = B[i];
            }
            if(B[i] < minB){
                minB = B[i];
            }
            promB += B[i];
        }
        #pragma omp single
        {
            //Calculamos el promedio de A y B
            promA = promA / N*N;
            promB = promB / N*N;
            //Y calculamos el escalar 
            T1 = ((maxA * maxB - minA * minB) / (promA * promB));
        }
    

        //Hacemos las multiplicaciones de matrices por bloques A y B
        #pragma omp for schedule(static) nowait
        for(i=0; i < N; i+=bs){
            for(j=0; j < N; j+=bs){
                for(k=0; k < N; k+=bs){
                    //Hacemos la multiplicacion de bloques
                    for (i2 = 0; i2 < bs; i2++){
                        auxI = (i+i2)*N;
                        for (j2 = 0; j2 < bs; j2++){
                            auxJ = (j+j2)*N;
                            for (k2 = 0; k2 < bs; k2++){
                                CxD[auxI + (j+j2)] += C[auxI + (k+k2)] * D[auxJ + (k+k2)];
                            }
                        }
                    }
                }
            }
        }
        //Hacemos las multiplicaciones de matrices por bloques C y D  
        #pragma omp for schedule(static) nowait
        for(i=0; i < N; i+=bs){
            for(j=0; j < N; j+=bs){
                for(k=0; k < N; k+=bs){
                    //Hacemos la multiplicacion de bloques
                    for (i2 = 0; i2 < bs; i2++){
                        auxI = (i+i2)*N;
                        for (j2 = 0; j2 < bs; j2++){
                            auxJ = (j+j2)*N;
                            for (k2 = 0; k2 < bs; k2++){
                                AxB[auxI + (j+j2)] += A[auxI + (k+k2)] * B[auxJ + (k+ k2)];
                            }
                        }
                    }
                }
            }
        }

        #pragma omp for schedule(static)  
        for(i=0; i < N*N; i++){
            R[i] = (AxB[i] * T1) + CxD[i]; 
        }
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
