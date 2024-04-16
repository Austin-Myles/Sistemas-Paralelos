//Ejercicio 1
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int main(int argc,char*argv[]){
   double *A;
   int i,j;
   int check = 1; 
   int numThreads = 0;
   int N= 0;

   if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((numThreads = atoi(argv[2])) < 0))
   {
      printf("\nUsar: %s N T\n  N: tamaño de la Matriz \n  T: cantidad de hilos\n ", argv[0]);
      exit(1);
   } 
   
   omp_set_num_threads(numThreads);

   //Aloca memoria para la matriz
   A=(double*)malloc(sizeof(double)*N*N);
   
   //Inicializa la matriz. Cada posicion debe quedar con el valor I*J
   // I => fila J=> columna.
   #pragma omp parallel for shared(A) private(i,j)      
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
         A[i*N+j]=i*j;
      }
   }  

   //Verifica el resultado
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
         check=check&&(A[i*N+j]==i*j);
      }
   }   

   if(check){
      printf("Resultado correcto\n");
   }else{
      printf("Resultado erroneo \n");
   }

   free(A);

   return(0);
}

