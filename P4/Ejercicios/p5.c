#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

/*Desarrolle un algoritmo paralelo que resuelva la expresión R = AB + CD + EF, donde A, B, C, D , 
E y F son matrices cuadradas de NxN. Ejecute para N = {512, 1024, 2048} con P={2,4,8,16}. 
 */

int main(int argc, char* argv[]){
	int i, j, k, numProcs, rank, n, stripSize, check=1;
	double * a, * b, *c, *d, *e, *f, *r;
    double *ab, *cd, *ef;
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

	// calcular porcion de cada worker
	stripSize = n / numProcs;

	// Reservar memoria
	if (rank == COORDINATOR) {
		a = (double*) malloc(sizeof(double)*n*n);
		c = (double*) malloc(sizeof(double)*n*n);
        e = (double*) malloc(sizeof(double)*n*n);
	    b = (double*) malloc(sizeof(double)*n*n);
        d = (double*) malloc(sizeof(double)*n*n);
        f = (double*) malloc(sizeof(double)*n*n);
        ab = (double*) malloc(sizeof(double)*n*n);
        cd = (double*) malloc(sizeof(double)*n*n);
        ef = (double*) malloc(sizeof(double)*n*n);
        r = (double*) malloc(sizeof(double)*n*n);
	}
	else  {
		a = (double*) malloc(sizeof(double)*n*stripSize);
		c = (double*) malloc(sizeof(double)*n*stripSize);
        e = (double*) malloc(sizeof(double)*n*stripSize);
        ab = (double*) malloc(sizeof(double)*n*stripSize);
        cd = (double*) malloc(sizeof(double)*n*stripSize);
        ef = (double*) malloc(sizeof(double)*n*stripSize);
        r = (double*) malloc(sizeof(double)*n*stripSize);
	}


    if (rank == COORDINATOR) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                a[i * n + j] = 1;
                b[i * n + j] = 1;
                c[i * n + j] = 1;
                d[i * n + j] = 1;
                e[i * n + j] = 1;
                f[i * n + j] = 1;
                r[i * n + j] = 0;
            }
        }
    }

	MPI_Barrier(MPI_COMM_WORLD);

	//Primero distribuimos para A*B;
	
	MPI_Scatter(a, stripSize * n, MPI_DOUBLE, a, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(b, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);


	/* computar multiplicacion parcial */
	for (i=0; i<stripSize; i++) {
		for (j=0; j<n ;j++) {
			ab[i*n+j]=0;
			for (k=0; k<n ;k++ ) { 
				ab[i*n+j] += (a[i*n+k]*b[j*n+k]); 
			}
		}
	}
	// vamos a usar un gather.
	MPI_Gather(ab, stripSize * n, MPI_DOUBLE, ab, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    //Segundo distribuimos para C*D;
	
	MPI_Scatter(c, stripSize * n, MPI_DOUBLE, c, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(d, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);


	/* computar multiplicacion parcial */
	for (i=0; i<stripSize; i++) {
		for (j=0; j<n ;j++) {
			cd[i*n+j]=0;
			for (k=0; k<n ;k++ ) { 
				cd[i*n+j] += (c[i*n+k]*d[j*n+k]); 
			}
		}
	}
	// vamos a usar un gather.
	MPI_Gather(cd, stripSize * n, MPI_DOUBLE, cd, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    //Segundo distribuimos para E*F;
	
	MPI_Scatter(e, stripSize * n, MPI_DOUBLE, e, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(f, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);


	/* computar multiplicacion parcial */
	for (i=0; i<stripSize; i++) {
		for (j=0; j<n ;j++) {
			ef[i*n+j]=0;
			for (k=0; k<n ;k++ ) { 
				ef[i*n+j] += (e[i*n+k]*f[j*n+k]); 
			}
		}
	}
	// vamos a usar un gather.
	MPI_Gather(ef, stripSize * n, MPI_DOUBLE, ef, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);


    //Sumamos los resultados
    for (i=0; i<stripSize ; i++) {
        for (j=0; j<n ;j++) {
            r[i*n+j] = ab[i*n+j] + cd[i*n+j] + ef[i*n+j];
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
	
	free(a);
	free(b);
	free(c);
    free(d);
    free(e);
    free(f);
    free(r);
    free(ab);
    free(cd);
    free(ef);


	return 0;
}