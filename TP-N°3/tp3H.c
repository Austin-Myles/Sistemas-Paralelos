#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define COORDINATOR 0

void matrix_blk(double *a, double *b, double *ab, int n, int block_size);

int main(int argc, char* argv[]){
    
	double commTimes[6], maxCommTimes[6], minCommTimes[6], commTime, totalTime;
	int i,j,k, numProcs, rank, n, fila, col, stripSize, provided;
    int auxi, auxj;
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
	    printf("\nUsar: %s size \n  size: Dimension de la matriz\nbs: Tamaño de bloque\n", argv[0]);
		exit(1);
	}

    //Inicializamos MPI
	MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &provided);

	MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (n % numProcs != 0) {
		printf("El tama�o de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}
    
    int block_size = atoi(argv[2]);
    int cant_hilos = 8;

    //Creamos las variables locales
    double localMaxA = -1;
    double localMaxB = -1;
    double localMinA = 10000;
    double localMinB = 10000;
    double localPromA = 0;
    double localPromB = 0;

	//Calculamos la porcion de cada worker
	stripSize = n / numProcs;

	// Alocamos las matrices
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

    //Inicializamos las matrices.
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

    //0-1 Primera comunicación.
    commTimes[0] = MPI_Wtime();

	//Distribuimos A y C en porciones para los procesos. B y D en su totalidad a todos los procesos.
	
	MPI_Scatter(A, stripSize * n, MPI_DOUBLE, A, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(B, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Scatter(C, stripSize * n, MPI_DOUBLE, C, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(D, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    
    commTimes[1] = MPI_Wtime();

    #pragma omp parallel private(i,j,k,auxi,auxj,fila,col) num_threads(cant_hilos) shared(A,B,C,D,AxB,CxD,R,stripSize,n,block_size)
    {
        //Calculamos el maximo, minimo y promedio de A.
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

        //Calculamos el maximo, minimo y promedio de B.
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
    
        //Hacemos las reducciones correspondientes y calculamos escalar.
        #pragma omp single
        {
            //2-3 Segundo tiempo de comunicación.
            commTimes[2] = MPI_Wtime();
            MPI_Allreduce(&localMaxA, &maxA, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
            MPI_Allreduce(&localMaxB, &maxB, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
            MPI_Allreduce(&localMinA, &minA, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
            MPI_Allreduce(&localMinB, &minB, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
            MPI_Allreduce(&localPromA, &promA, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            MPI_Allreduce(&localPromB, &promB, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            commTimes[3] = MPI_Wtime();
            promA = promA / (n*n);
            promB = promB / (n*n);    
            T1 = ((maxA * maxB - minA * minB) / (promA * promB));
        }

        //Hacemos las multiplicaciones de matrices por bloques A y B
        #pragma omp for schedule(static) nowait
        for(i = 0; i < stripSize; i += block_size){
            fila = i * n;
            for(j = 0; j < n; j += block_size){
                col = j * n;
                for(k = 0; k < n; k += block_size){
                    matrix_blk(&A[fila + k], &B[col + k], &AxB[fila + j], n, block_size);
                }
            }
        }

        //Hacemos las multiplicaciones de matrices por bloques C y D
        #pragma omp for schedule(static) nowait
        for(i = 0; i < stripSize; i += block_size){
            fila = i * n;
            for(j = 0; j < n; j += block_size){
                col = j * n;
                for(k = 0; k < n; k += block_size){
                    matrix_blk(&A[fila + k], &B[col + k], &AxB[fila + j], n, block_size);
                }
            }
        }

        //Calculamos el resultado total
        #pragma omp for schedule(static)
        for (i=0; i<stripSize ; i++) {
            auxi = i * n;
            for (j=0; j<n ;j++) {
                R[auxi+j] += (AxB[auxi+j] * T1) + CxD[auxi+j];
            }
        }
    }

    commTimes[4] = MPI_Wtime();
    MPI_Gather(R, stripSize * n, MPI_DOUBLE, R, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    commTimes[5] = MPI_Wtime();

    //Obtenemos el tiempo total.
    
	MPI_Reduce(commTimes, minCommTimes, 6, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 6, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank == COORDINATOR) {
        //Calculamos el tiempo total que tomo el algoritmo.
        totalTime = maxCommTimes[5] - minCommTimes[0];
		commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]) + (maxCommTimes[5] - minCommTimes[4]);		

		printf("%lf,%lf",totalTime,commTime);
        
	}

    //Liberamos memoria
    if (rank == COORDINATOR) {
        free(A);
        free(C);
        free(R);
        free(AxB);
        free(CxD);
    } else {
        free(A);
        free(C);
        free(R);
        free(AxB);
        free(CxD);
    }
    free(B);
    free(D);
    
	return 0;
}
    //Procemiento encargado de realizar la multiplicacion por bloques
    void matrix_blk(double *a, double *b, double *ab, int n, int block_size) 
    {
        int i, j, k, fila, col;
        double suma;
        
        for(i = 0; i < block_size; i++){
            fila = i * n;
            for(j = 0; j < block_size; j++){
                suma = 0;
                col = j * n;
                for(k = 0; k < block_size; k++){
                    suma += a[fila + k] * b[col + k];
                }
                ab[fila + j] += suma;
            }
        }
    }