#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

void matrix_multiply(double *a, double *b, double *result, int n, int stripSize) {
    int i, j, k;
    for (i = 0; i < stripSize; i++) {
        for (j = 0; j < n; j++) {
            result[i * n + j] = 0;
            for (k = 0; k < n; k++) {
                result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    int i, j, numProcs, rank, n, stripSize, check = 1;
    double *a, *b, *c, *d, *e, *f, *r;
    double *ab, *cd, *ef, *partial_result;

    if ((argc != 2) || ((n = atoi(argv[1])) <= 0)) {
        printf("\nUsar: %s size \n  size: Dimension de la matriz\n", argv[0]);
        exit(1);
    }

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (n % numProcs != 0) {
        printf("El tamaño de la matriz debe ser múltiplo del número de procesos.\n");
        MPI_Finalize();
        exit(1);
    }

    printf("n: %d, numProcs: %d, rank: %d\n", n, numProcs, rank);
    stripSize = n / numProcs;

    // Reservar memoria
    if (rank == COORDINATOR) {
        a = (double*) malloc(sizeof(double) * n * n);
        b = (double*) malloc(sizeof(double) * n * n);
        c = (double*) malloc(sizeof(double) * n * n);
        d = (double*) malloc(sizeof(double) * n * n);
        e = (double*) malloc(sizeof(double) * n * n);
        f = (double*) malloc(sizeof(double) * n * n);
        r = (double*) malloc(sizeof(double) * n * n);
        ab = (double*) malloc(sizeof(double) * n * n);
        cd = (double*) malloc(sizeof(double) * n * n);
        ef = (double*) malloc(sizeof(double) * n * n);
    } else {
        a = (double*) malloc(sizeof(double) * n * stripSize);
        c = (double*) malloc(sizeof(double) * n * stripSize);
        e = (double*) malloc(sizeof(double) * n * stripSize);
        ab = (double*) malloc(sizeof(double) * n * stripSize);
        cd = (double*) malloc(sizeof(double) * n * stripSize);
        ef = (double*) malloc(sizeof(double) * n * stripSize);
        r = (double*) malloc(sizeof(double) * n * stripSize);
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

    // Distribuir matrices y calcular ab
    MPI_Scatter(rank == COORDINATOR ? a : MPI_IN_PLACE, stripSize * n, MPI_DOUBLE, a, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(b, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    matrix_multiply(a, b, ab, n, stripSize);

    // Distribuir matrices y calcular cd
    MPI_Scatter(rank == COORDINATOR ? c : MPI_IN_PLACE, stripSize * n, MPI_DOUBLE, c, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(d, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    matrix_multiply(c, d, cd, n, stripSize);

    // Distribuir matrices y calcular ef
    MPI_Scatter(rank == COORDINATOR ? e : MPI_IN_PLACE, stripSize * n, MPI_DOUBLE, e, stripSize * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(f, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    matrix_multiply(e, f, ef, n, stripSize);

    // Sumar ab, cd y ef en cada proceso
    partial_result = (double*) malloc(sizeof(double) * n * stripSize);
    for (i = 0; i < stripSize; i++) {
        //printf("%f %f %f\n", ab[i * n], cd[i * n], ef[i * n]);
        for (j = 0; j < n; j++) {
            partial_result[i * n + j] = ab[i * n + j] + cd[i * n + j] + ef[i * n + j];
        }
    }

    // Reducir las sumas parciales en r
    MPI_Reduce(rank == COORDINATOR ? MPI_IN_PLACE : partial_result, r, n * n, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == COORDINATOR) {
        // Check results
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                check = check && (r[i * n + j] == 3);
            }
        }

        if (check) {
            printf("Multiplicación de matrices resultado correcto\n");
        } else {
            printf("Multiplicación de matrices resultado erróneo\n");
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
    } else {
        free(a);
        free(c);
        free(e);
        free(partial_result);
        free(ab);
        free(cd);
        free(ef);
    }

    return 0;
}
