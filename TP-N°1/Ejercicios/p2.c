#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <sys/time.h>

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

int main(int argc,char*argv[]){

    //Dimensión de las matrices
    int N = 0;

    //Creamos las matrices
    double *A,*B,*C,*D,*R;

    //Creamos las variables maximas, minimas y promedios y les declaramos valores.
    double maxA = -1;
    double maxB = -1;
    double minA = 10000; 
    double minB = 10000; 
    double promA = 0; 
    double promB = 0;

    double timetick;

    //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) )
    {
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }

    //Aloca memoria para los matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);
    R=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices en 1 y R en 0
    for(int i=0;i< N*N ;i++){
            A[i] = 1;
            B[i] = 1;
            C[i] = 1;
            D[i] = 1;
            R[i] = 0;
    }

    //Obtenemos el max, min y promedio de A y B
    for(int i=0; i<N*N; i++){
        if(A[i] > maxA){
            maxA = A[i];
        }
        if(A[i] < minA){
            minA = A[i];
        }
        promA += A[i];

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
    
    timetick = dwalltime();

    for(int i=0; i < N; i++){
        for(int j=0; j < N; j++){
            R[i*N+j] += ((maxA * maxB - minA * minB) / (promA * promB)) * (A[i*N+j] * B[i*N+j]) + (C[i*N+j] * D[i*N+j]);
        } 
    }

    double time = dwalltime() - timetick;

    printf("Ecuación sobre cada posición de la matriz R de dimensión NxN %d. Tiempo en segundos %f\n",N,time);

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    return(0);
}
