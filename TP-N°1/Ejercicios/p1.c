#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TIMES 100
#define N 10000000

#define A 1.0
#define B -4.0000000
#define C 3.9999999


/* Time in seconds from some point in the past */
double dwalltime();


void dbl_solve(double a, double b, double c);
void flt_solve(float a, float b, float c);

int main(void)
{
    float fa = A;
    float fb = B;
    float fc = C;
    double da = A;
    double db = B;
    double dc = C;
    flt_solve(fa, fb, fc);
    dbl_solve(da, db, dc);
    return 0;
}

void dbl_solve(double a, double b, double c)
{
    double d = pow(b,2) - 4.0*a*c;
    double sd = sqrt(d);
    double r1 = (-b + sd) / (2.0*a);
    double r2 = (-b - sd) / (2.0*a);
    printf("Soluciones Double: %.5f\t%.5f\n", r1, r2);
}

void flt_solve(float a, float b, float c)
{
    float d = pow(b,2) - 4.0*a*c;
    float sd = sqrt(d);
    float r1 = (-b + sd) / (2.0*a);
    float r2 = (-b - sd) / (2.0*a);
    printf("Soluciones Float: %.5f\t%.5f\n", r1, r2);
}   
