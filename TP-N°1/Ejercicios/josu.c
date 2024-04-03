#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/* Provided time measurement function */
double dwalltime();

int sizes[4] = {512, 1024, 2048, 4096};
int N = 512;

double *A, *B, *C, *D, *R;

int resolve()
{
    double total_time, tick;
    int i, j;

    tick = dwalltime();

    // computation of (MaxA x MaxB - MinA x MinB) / (AvgA x AvgB)
    double max_a, max_b, min_a, min_b, avg_a, avg_b, scalar;
    max_a = max_b = -1.0;
    min_a = min_b = 1000000.0;
    avg_a = avg_b = 0.0;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (A[i * N + j] > max_a)
                max_a = A[i * N + j];
            if (A[i * N + j] < min_a)
                min_a = A[i * N + j];
            avg_a += A[i * N + j];
        }
    }

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (B[i * N + j] > max_b)
                max_b = B[i * N + j];
            if (B[i * N + j] < min_b)
                min_b = B[i * N + j];
            avg_b += B[i * N + j];
        }
    }
    avg_a /= N * N;
    avg_b /= N * N;
    scalar = (max_a * max_b - min_a * min_b) / (avg_a * avg_b);

    // computation of R = [C x D]
    double sum;
    int k;
//     //for (i = 0; i < N; i++)
//     // {
//         // for (j = 0; j < N; j++)
//         {
//             sum = 0;
//             for (k = 0; k < N; k++)
//             {
//                 sum += C[i * N + k] * D[k + j * N];
//             }
//             R[i * N + j] = sum;
//         }
//    // }

    // computation of R = scalar * [A x B] + R
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            sum = 0;
            for (k = 0; k < N; k++)
            {
                sum += A[i * N + k] * B[k + j * N] + C[i * N + k] * D[k + j * N];
            }
            R[i * N + j] += sum * scalar;
        }
    }

    total_time = dwalltime() - tick;

    printf("Total time: %f\n", total_time);

    // Result check
    int check = 1;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            check = check && (R[i * N + j] == N);
        }
    }

    printf("Result check: %s\n", check ? "OK" : "ERROR");

    return 0;
}

void init_matrix(double *matrix)
{
    int i, j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            matrix[i * N + j] = 1;
        }
    }
}

int main(void)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        N = sizes[i];

        printf("Computing for N = %d\n", N);

        A = (double *)malloc(sizeof(double) * N * N);
        B = (double *)malloc(sizeof(double) * N * N);
        C = (double *)malloc(sizeof(double) * N * N);
        D = (double *)malloc(sizeof(double) * N * N);
        R = (double *)malloc(sizeof(double) * N * N);

        init_matrix(A);
        init_matrix(B);
        init_matrix(C);
        init_matrix(D);

        resolve();

        free(A);
        free(B);
        free(C);
        free(D);
        free(R);
    }

    return 0;
}

double dwalltime()
{
    double sec;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}
