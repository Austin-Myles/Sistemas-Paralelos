#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> 
#include <time.h>

#define COORDINATOR 0

/*Desarrolle un algoritmo paralelo que dado un vector V de tamaño N obtenga el valor máximo,
el valor mínimo y valor promedio de sus elementos. Ejecute para P={2,4,8,16} variando el
valor de N.
 */

int main(int argc, char* argv[]){
	int i, j, k, numProcs, rank, n, stripSize, check=1;
	double *a;
    double max = -1;
    double min = 10000; 
    double prom = 0;
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
	}
	else  {
		a = (double*) malloc(sizeof(double)*n*stripSize);
	}

    //Inicializamos los datos

    if (rank == COORDINATOR) {
        srand(time(NULL)); // Seed the random number generator
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                a[i * n + j] = rand() % 100; // Generate a random number between 0 and 99
            }
        }
    }
    double localmax = -1;
    double localmin = 10000; 
    double localprom = 0;
	MPI_Barrier(MPI_COMM_WORLD);
	//Primero distribuimos para A*B;
	
	MPI_Scatter(a, stripSize * n, MPI_DOUBLE, a, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    //Calculamos el maximo, minimo y promedio de los elementos del vector
    for (i = 0; i < n; i++) {
        if (a[i] > localmax) {
            localmax = a[i];
        }
        if (a[i] < localmin) {
            localmin = a[i];
        }
        localprom += a[i];
    }

    MPI_Reduce(&localmax, &max, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localmin, &min, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&localprom, &prom, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank==COORDINATOR) {
        prom = prom / (n);
        printf("Max: %f, Min: %f, Prom: %f\n", max, min, prom);
        printf("Cantidad de procesos: %d\nDimesión del vector: %d\n", numProcs, n);
        printf("Fin :3 \n");

	}
	
	free(a);

	return 0;
}