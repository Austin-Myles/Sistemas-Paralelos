#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <sys/time.h>

//Creamos las matrices
double *A,*B,*C,*D,*R;

// Procedimiento para calcular el máximo, mínimo y promedio 
void calcularMaxMinProm(double *array, int tam, double *max, double *min, double *promedio) {
    *max = array[0];
    *min = array[0];
    *promedio = 0;

    for(int i = 0; i < tam; i++) {
        if(array[i] > *max) {
            *max = array[i];
        }
        if(array[i] < *min) {
            *min = array[i];
        }
        *promedio += array[i];
    }

    *promedio /= tam;
}

//Para realizar la Ecuación.
void operacionMatrices(double *A, double *B, double *C, double *D, double *R, int N, double maxA, double maxB, double minA, double minB, double promA, double promB) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            for(int k = 0; k < N; k++) {
                R[i*N+j] += ((maxA * maxB - minA * minB) / (promA * promB)) * (A[i*N+k] * B[k + N*j]) + (C[i*N+j] * D[k+N*j]);
            }
        } 
    }
}

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

    timetick = dwalltime();

    // Calcular el máximo, mínimo y promedio de A y B
    calcularMaxMinProm(A, N*N, &maxA, &minA, &promA);
    calcularMaxMinProm(B, N*N, &maxB, &minB, &promB);

    // Realizar la ecuación sobre R
    operacionMatrices(A, B, C, D, R, N, maxA, maxB, minA, minB, promA, promB);

    double time = dwalltime() - timetick;

    printf("Ecuación sobre cada posición de la matriz R de dimensión NxN %d. Tiempo en segundos %f\n",N,time);

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    return(0);
}
