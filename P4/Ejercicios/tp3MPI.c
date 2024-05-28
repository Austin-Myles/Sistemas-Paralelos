#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

/*Desarrolle un algoritmo paralelo que resuelva la expresión R = AB + CD + EF, donde A, B, C, D , 
E y F son matrices cuadradas de NxN. Ejecute para N = {512, 1024, 2048} con P={2,4,8,16}. 
 */

int main(int argc, char* argv[]){
	int i, j, k, numProcs, rank, n, stripSize, check=1;
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
	if ((argc != 2) || ((n = atoi(argv[1])) <= 0) ) {
	    printf("\nUsar: %s size \n  size: Dimension de la matriz\n", argv[0]);
		exit(1);
	}

	MPI_Init(&argc,&argv);

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
        B=(double*)malloc(sizeof(double)*n*n);
        C=(double*)malloc(sizeof(double)*n*n);
        D=(double*)malloc(sizeof(double)*n*n);
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


    if (rank == COORDINATOR) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                A[i * n + j] = 1;
                B[i * n + j] = 1;
                C[i * n + j] = 1;
                D[i * n + j] = 1;
                R[i * n + j] = 0;
                AxB[i * n + j] = 0;
                CxD[i * n + j] = 0;
            }
        }
    }

	MPI_Barrier(MPI_COMM_WORLD);

	//Primero distribuimos para A, B, C y D para Max, Min y Promedio;
	
	MPI_Scatter(A, stripSize * n, MPI_DOUBLE, A, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(B, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    for(i=0;i<stripSize;i++){
        for(j=0;j<n;j++){
            if(a[i*n+j]>localMaxA){
                localMaxA = a[i*n+j];
            }
            if(a[i*n+j]<localMinA){
                localMinA = a[i*n+j];
            }
            localPromA += a[i*n+j];
        }
    }

    for(i=0;i<stripSize;i++){
        for(j=0;j<n;j++){
            if(b[i*n+j]>localMaxB){
                localMaxB = c[i*n+j];
            }
            if(b[i*n+j]<localMinB){
                localMinB = b[i*n+j];
            }
            localPromB += b[i*n+j];
        }
    }

    MPI_Reduce(&localMaxA, &maxA, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localMaxB, &maxB, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localMinA, &minA, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localMinB, &minB, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localPromA, &promA, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localPromB, &promB, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == COORDINATOR){
        promA = promA / (n*n);
        promB = promB / (n*n);
        T1 = ((maxA * maxB - minA * minB) / (promA * promB));
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
	/* computar multiplicacion parcial */
	for (i=0; i<stripSize; i++) {
		for (j=0; j<n ;j++) {
			for (k=0; k<n ;k++ ) { 
				AxB[i*n+j] += (A[i*n+k]*B[j*n+k]); 
			}
		}
	}
	// vamos a usar un gather.
	MPI_Gather(AxB, stripSize * n, MPI_DOUBLE, AxB, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    //Segundo distribuimos para C*D;
    
    MPI_Scatter(C, stripSize * n, MPI_DOUBLE, C, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(D, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	/* computar multiplicacion parcial */
	for (i=0; i<stripSize; i++) {
		for (j=0; j<n ;j++) {
			for (k=0; k<n ;k++ ) { 
				CxD[i*n+j] += (C[i*n+k]*D[j*n+k]); 
			}
		}
	}
	// vamos a usar un gather.
	MPI_Gather(CxD, stripSize * n, MPI_DOUBLE, CxD, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    //Sumamos los resultados
    for (i=0; i<stripSize ; i++) {
        for (j=0; j<n ;j++) {
            R[i*n+j] = (AxB[i*n+j] * T1) + CxD[i*n+j];
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////


	MPI_Finalize();

	if (rank==COORDINATOR) {

		// Check results
		for(i=0;i<n;i++)
			for(j=0;j<n;j++)
				check=check&&(r[i*n+j]==(3*n));

		if(check){
			printf("Multiplicacion de matrices resultado correcto\n");
		}else{
			printf("Multiplicacion de matrices resultado erroneo\n");
		}
		
        printf("Fin :3 \n");

	}


	return 0;
}