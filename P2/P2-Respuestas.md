# Práctica Nro. 1

_Optimización de algoritmos secuenciales_ 

1.  Desarrolle un algoritmo paralelo que compute una suma de vectores: 𝐴𝑖 = 𝐵𝑖 + 𝐶𝑖. Para ello, considere lo siguiente: 
    - Identifique  las  regiones  de  código  que  pueden  ejecutarse  en  paralelo  y  las  que no.  ¿Existen  dependencias? 
    - Analice  cómo  distribuir  el  trabajo  entre  los  hilos.  ¿El  cómputo  es  regular?  ¿La  arquitectura  es homogénea?
    
    Mida el tiempo de ejecución para diferentes valores de N y T={2,4,8}. Analice el rendimiento.

    - La ídea principal es dividir la carga de trabajo designandole la suma a cada thread de manera individual, dependiendo de la cantidad de threads que hay y segun el tamaño del vector a sumar, cada Thread se encargara de sumar la i-esima posición del vector B y C en el vector A. 
    - 

2. Desarrolle  un  algoritmo  paralelo  que  compute  la  multiplicación  de  matrices  cuadradas  de  NxN.  Primero, considere  a  la  versión  optimizada  del  ejercicio  8  de  la  práctica  anterior  como  algoritmo  base.  Luego, paralelice la versión que computa por bloques. Mida el tiempo de ejecución para N={512, 1024, 2048, 4096} y T={2,4,8}. Analice el rendimiento. 
    -  

3. Desarrolle un algoritmo paralelo que cuente la cantidad de veces que un elemento X aparece dentro de un vector de N elementos enteros.  Al finalizar, la cantidad de ocurrencias del elemento X debe quedar en una variable llamada ocurrencias. Para la sincronización emplee mutex-locks. Pruebe con diversos tamaños de N y T={2,4,8}. Analice el rendimiento.
    -