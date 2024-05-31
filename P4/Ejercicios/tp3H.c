#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define COORDINATOR 0

void matrix_mul(double *a, double *b, double *ab, int n, int stripSize) {
    int i, j, k, auxi;
    #pragma opm parallel for num_threads(threads) reduction(+:auxi) shared(ab,a,b) private(i,j,k,auxi) schedule(static)
    for (i = 0; i < stripSize; i++) {
        auxi = i * n;
        for (j = 0; j < n; j++) {
            ab[auxi + j] = 0;
            for (k = 0; k < n; k++) {
                ab[auxi + j] += a[auxi + k] * b[k * n + j];
            }
        }
    }
}

/*Desarrolle un algoritmo paralelo que resuelva la expresión R = AB + CD + EF, donde A, B, C, D , 
E y F son matrices cuadradas de NxN. Ejecute para N = {512, 1024, 2048} con P={2,4,8,16}. 
 */

int main(int argc, char* argv[]){
    
	double commTimes[2], maxCommTimes[2], minCommTimes[2], commTime, totalTime;
	int i, j, k, numProcs, rank, n, stripSize, threads, provided;
    int auxi;
	double *A,*B,*C,*D,*AxB,*CxD,*R;   
    double maxA = -1;
    double maxB = -1;
    double minA = 10000; 
    double minB = 10000; 
    double promA = 0; 
    double promB = 0;
    double T1 = 0;

	MPI_Status status;

	/* Lee par�metros de la l�nea de comando */
	if ((argc != 3) || ((n = atoi(argv[1])) <= 0) ) {
	    printf("\nUsar: %s size \n  size: Dimension de la matriz\n T: Cant Threads", argv[0]);
		exit(1);
	}
    threads = atoi(argv[2]);

	MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &provided);

	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (n % numProcs != 0) {
		printf("El tama�o de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

    //Creamos las variables locales
    double localMaxA = -1;
    double localMaxB = -1;
    double localMinA = 10000;
    double localMinB = 10000;
    double localPromA = 0;
    double localPromB = 0;


	//Calculamos la porcion de cada worker
	stripSize = n / numProcs;

	// Reservar memoria
	if (rank == COORDINATOR) {
        A=(double*)malloc(sizeof(double)*n*n);
        C=(double*)malloc(sizeof(double)*n*n);
        R=(double*)malloc(sizeof(double)*n*n);
        AxB=(double*)malloc(sizeof(double)*n*n);
        CxD=(double*)malloc(sizeof(double)*n*n);
	}
	else  {
		A = (double*) malloc(sizeof(double)*n*stripSize);
		C = (double*) malloc(sizeof(double)*n*stripSize);
        R = (double*) malloc(sizeof(double)*n*stripSize);
        AxB = (double*) malloc(sizeof(double)*n*stripSize);
        CxD = (double*) malloc(sizeof(double)*n*stripSize);
	}
    
    B=(double*)malloc(sizeof(double)*n*n);
    D=(double*)malloc(sizeof(double)*n*n);

    if (rank == COORDINATOR) {
        for (i = 0; i < n; i++) {
            auxi = i * n;
            for (j = 0; j < n; j++) {
                A[auxi + j] = 1;
                B[auxi + j] = 1;
                C[auxi + j] = 1;
                D[auxi + j] = 1;
                R[auxi + j] = 0;
                AxB[auxi + j] = 0;
                CxD[auxi + j] = 0;
            }
        }
    }

	MPI_Barrier(MPI_COMM_WORLD);

    commTimes[0] = MPI_Wtime();

	//Primero distribuimos para A y B para calcular Max, Min y Promedio;
	
	MPI_Scatter(A, stripSize * n, MPI_DOUBLE, A, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(B, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    #pragma opm parallel num_threads(threads) shared(A,B,C,D,AxB,CxD) private(i,j,k,auxi)
    {
        #pragma omp for schedule(static) reduction(max:localMaxA) reduction(min:localMinA) reduction(+:localPromA)
        for(i=0;i<stripSize;i++){
            auxi = i * n;
            for(j=0;j<n;j++){
                if(A[auxi+j]>localMaxA){
                    localMaxA = A[auxi+j];
                }
                if(A[auxi+j]<localMinA){
                    localMinA = A[auxi+j];
                }
                localPromA += A[auxi+j];
            }
        }

        #pragma omp for schedule(static) reduction(max:localMaxB) reduction(min:localMinB) reduction(+:localPromB)
        for(i=0;i<stripSize;i++){
            auxi = i * n;
            for(j=0;j<n;j++){
                if(B[i*n+j]>localMaxB){
                    localMaxB = B[i*n+j];
                }
                if(B[i*n+j]<localMinB){
                    localMinB = B[i*n+j];
                }
                localPromB += B[i*n+j];
            }
        }

        MPI_Reduce(&localMaxA, &maxA, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
        MPI_Reduce(&localMaxB, &maxB, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
        MPI_Reduce(&localMinA, &minA, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
        MPI_Reduce(&localMinB, &minB, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
        MPI_Reduce(&localPromA, &promA, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
        MPI_Reduce(&localPromB, &promB, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        //El coordinador calcula el escalar.
        #pragma omp master
        {
            promA = promA / (n*n);
            promB = promB / (n*n);
            T1 = ((maxA * maxB - minA * minB) / (promA * promB));
        }

        MPI_Barrier(MPI_COMM_WORLD);

        //////////////////////////////////////////////////////////////////////////////////////////////////////

        matrix_mul(A, B, AxB, n, stripSize);

        MPI_Gather(AxB, stripSize * n, MPI_DOUBLE, AxB, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

        //Segundo distribuimos para C*D;
        
        MPI_Scatter(C, stripSize * n, MPI_DOUBLE, C, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
        MPI_Bcast(D, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

        matrix_mul(C, D, CxD, n, stripSize);

        MPI_Gather(CxD, stripSize * n, MPI_DOUBLE, CxD, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

        //Sumamos los resultados
        #pragma omp for schedule(static)
        for (i=0; i<stripSize ; i++) {
            for (j=0; j<n ;j++) {
                R[auxi+j] = (AxB[auxi+j] * T1) + CxD[auxi+j];
            }
        }

        MPI_Gather(R, stripSize * n, MPI_DOUBLE, R, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    commTimes[1] = MPI_Wtime();

	MPI_Reduce(commTimes, minCommTimes, 2, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 2, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);


	MPI_Finalize();

	if (rank == COORDINATOR) {

        totalTime = maxCommTimes[1] - minCommTimes[0];
        printf("Multiplicacion de matrices (N=%d)\tCantidad de procesos (P=%d)\tTiempo total=%lf\n",n,numProcs,totalTime);

	}


	return 0;
}