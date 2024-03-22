# Práctica Nro. 1

_Optimización de algoritmos secuenciales_ 

1. El  algoritmo  fib.c  resuelve  la  serie  de  Fibonacci,  para  un  número  N  dado,  utilizando  dos  métodos: recursivo  e  iterativo.  Analice  los  tiempos  de  ejecución  de ambos  métodos  ¿Cuál  es  más  rápido?  ¿Por qué? \
Nota: ejecute con N=1..50.  \

    1. Segun los tiempos de ejecución el más rapido es el método iterativo, puede ocurrir que con parametros pequeños menores a 10 el metodo iterativo pueda verse superado por el metodo recursivo, pero una vez se utilizan parametros superando los 10 se empieza a notar la diferencia en velocidad. \
    El metodo iterativo ocurre dentro de un for, mientras que el recursivo ocurre dentro de un procedimiento que se tendra que ejecutar N veces.

2. El algoritmo funcion.c resuelve, para un x dado, la siguiente sumatoria: ![alt text](image.png) \
    El algoritmo compara dos alternativas de solución. ¿Cuál de las dos formas es más rápido? ¿Por qué?

    2. El tiempo de ejecución de la función calculada cada vez es mayor debido a que por cada iteración se calcula toda la expresión de la sumatoria en la variable de resultado, mientras que en el caso de la función calculada que solo realiza la expresión una vez y luego es referenciada en cada iteración del for.

3. El algoritmo instrucciones.c compara el tiempo de ejecución de las operaciones básicas: suma (+), resta 
(-), multiplicación (*) y división (/), para dos operandos dados x e y. ¿Qué análisis se puede hacer de 
cada operación? ¿Qué ocurre si x e y son potencias de 2? \
    3. Por cada operación se va a iterar 1M de veces realizando la operación, solamente se realiza la operación y se la asigna a la variable resultado. \
    En caso de operaciones con potencias de 2 puede resultar ser mas eficiente en divisiones y multiplicaciones(Segun fuentes confiables).

4. En  función  del  ejercicio  anterior  analice  el  algoritmo  instrucciones2.c  que  resuelve  una  operación 
binaria (dos operandos) con dos operaciones distintas.
    4. Ambas operaciones se realizan dentro del for al igual que en el ejercicio anterior, la diferencia recae en que la división se realiza con un entero(es 5 aunque este declarado con un double), la división entre "enteros" requiere menos recursos y ciclos de reloj, mientras que la multiplicación se realiza con un double (0.2), este tipo de operaciones requieren más ciclos de reloj y más recursos de computo.

5. Investigue  en  la  documentación  del  compilador  o  a  través  de  Internet  qué  opciones  de  optimización 
ofrece  el  compilador  gcc  (flag  O).  Compile  y  ejecute  el  algoritmo  matrices.c,  el  cual  resuelve  una 
multiplicación  de  matrices  de  NxN.  Explore  los  diferentes  niveles  de  optimización  para  distintos 
tamaños  de  matrices.  ¿Qué  optimizaciones  aplica  el  compilador?  ¿Cuál  es  la  ganancia  respecto  a  la 
versión sin optimización del compilador? ¿Cuál es la ganancia entre los distintos niveles?

    5.  **0**: Optimización por defecto (0 optimización).\

        **1, 2, 3**: Cuanto mayor sea el número, mayor sera el nivel optimización.\

        **S**: Optimizar el tamaño del archivo.\

        **fast**: Funciona similar al nivel 3, fast hace caso omiso del estricto cumplimiento de las normas para permitir optimizaciones adicionales.\

        **g**: Optimizar la experiencia de depuración. \

        (Fuentes access.redhat.com) \

        Dependiendo del código sera mas viable optimizar o no con el compilador. El rendimiento/eficiencia dependera mucho de la complejidad del código y del tamaño de este, el grado de eficiencia sera muy diferente si el código es simple y corto, incluso puede ser contraproducente utilizar un metodo de optimización en códigos de ese estilo. Ya en códigos de mayor complejidad puede resultar optimo (valga la redundancia) usar un optimizador en el compilador. 
    
6.  Dada la ecuación cuadrática:  x2 − 4.0000000 x + 3.9999999 = 0, sus raíces son r1 = 2.000316228  y r2 = 1.999683772 (empleando 10 dígitos para la parte decimal).  

    a. El  algoritmo  quadratic1.c  computa  las  raíces  de  esta  ecuación  empleando  los  tipos  de  datos float y double. Compile y ejecute el código. ¿Qué diferencia nota en el resultado? 

    - La diferencia radica en que ambos resultados varian en los últimos digitos, debido a que las soluciones **double** muchas veces son más precisas que las soluciones **float** que tendra una tendencia a redondear el resultado mientras mas cercano este al 0.

    b. El algoritmo quadratic2.c computa las raíces de esta ecuación, pero en forma repetida. Compile y ejecute el código variando la constante TIMES. ¿Qué diferencia nota en la ejecución?  

    - La diferencia que es notable es la reducción considerable del tiempo que toma el algoritmo para resolver las soluciones en Double y Float. Float siempre toma mas tiempo.
    En el caso de Times = 100 (Double = 48.584841 y Float = 52.846624), con Times = 10 (Double = 4.323746 y Float = 5.023306) y Times = 1 (Double = 0.467906 y Float = 0.477569)
    
    c. El algoritmo quadratic3.c computa las raíces de esta ecuación, pero en forma repetida. Compile y  ejecute  el  código  variando  la  constante  TIMES.  ¿Qué  diferencia  nota en  la  ejecución?  ¿Qué diferencias puede observar en el código con respecto a quadratic2.c? 

    -Al igual que en el inciso anterior, las diferencias al cambiar la constante TIMES reduce considerablemente el tiempo reuquerido para ambas soluciones. 
     - (TIMES = 100):
        Tiempo requerido solucion Double: 40.462592 
        Tiempo requerido solucion Float: 27.412683
     - (TIMES = 10):
        Tiempo requerido solucion Double: 4.029655 
        Tiempo requerido solucion Float: 2.728085
     - (TIMES = 1):
        Tiempo requerido solucion Double: 0.392069
        Tiempo requerido solucion Float: 0.264048
    A comparación con el inciso anterior, las soluciones tomen menos tiempo, ademas de que los valores A, B y C, se definen en constantes en **double** y en **float**. Supongo que se reduce el tiempo de ejecución ya que no es necesario pasar el valor de double a float, por ende al ya estar declarado como float en la constante y asignarlo a una variable hace que el algoritmo no se demore en realizar la conversión...

7. Analice los algoritmos iterstruc1.c e iterstruc2.c que resuelven una multiplicación de matrices utilizando dos estructuras de control distintas. ¿Cuál de las dos estructuras de control tiende a acelerar el cómputo? Compile con y sin opciones de optimización del compilador.
    - **SIN OPTIMIZAR**: Despues de varias ejecuciones sin utilizar opciones de optimización llegue a la conclusión de que la estructura que acelera el computo es el iterstruc2.c, su utilización del While por ejemplo con parametro 1000 toma 4.257286 segundos, a diferencia de con for que toma 4.339727.

    - **CON OPTIMIZACIÓN (-fast)**: No hubo mucha diferencia, los parametros usados fueron 1000 y 2000. En ambos casos While se vio superior a For en velocidad de ejecución.
        -   Tiempo For en segundos 4.766421.
        -   Tiempo For en segundos 32.160391.
        -   Tiempo While en segundos 3.991992.
        -   Tiempo While en segundos 31.951677.

8. Analice el algoritmo matrices.c. ¿Dónde cree que se producen demoras? ¿Cómo podría optimizarse el código? Al menos, considere los siguientes aspectos: 
- Explotación  de  localidad  de  datos  a  través  de  reorganización  interna  de matrices  A,  B  o  C (según corresponda). 
- El  uso  de  Setters  y  getters  es  una  buena  práctica  en  la  programación  orientada  a  objetos. ¿Tiene sentido usarlos en este caso? ¿cuál es su impacto en el rendimiento? 
- ¿Hay  expresiones  en  el  cómputo  que  pueden  refactorizarse  para  no  ser  computadas  en forma repetida?
- En lugar de ir acumulando directamente sobre la posición C[i,j] de la matriz resultado (línea 72), pruebe  usar  una  variable local individual y al finalizar el bucle más interno, asigne su valor a C[i,j]. ¿Esta modificación impacta en el rendimiento? ¿Por qué?

    - Luego de analizar segun los aspectos planteados en el ejercicio, se hizo una refactorización del código en la cual se eliminaron por completo los metodos **Setter** y **Getter**, ademas de cambiar las expresiones como la ubicada en la linea 72, ademas de la eliminación del **for loop** de la linea 57. Se accede ademas directamente a los índices de los elementos de las matrices. Reducimos la complejidad y la utilización de elementos innecesarios para este codigo.

9. Analice  y  describa brevemente  cómo funciona  el  algoritmo  mmblk.c  que resuelve  la  multiplicación  de matrices  cuadradas  de  NxN  utilizando  una  técnica  de  multiplicación  por  bloques.  Luego,  ejecute el algoritmo utilizando distintos tamaños de matrices y distintos tamaños de bloque (pruebe con valores que sean  potencia de  2; p.e.  N={512,1024,2048} y TB={16,32,64,128}). Finalmente, compare los tiempos con respecto a la multiplicación de matrices optimizada del ejercicio anterior. Según el tamaño de  las matrices  y  de bloque  elegido,  responda:  ¿Cuál  es  más  rápido?  ¿Por  qué?  ¿Cuál  sería  el  tamaño de  bloque  óptimo  para  un  determinado  tamaño  de  matriz?  ¿De  qué  depende  el  tamaño  de  bloque óptimo para un sistema?

    -  mmblk.c comienza haciendo las verificaciones de parametros, aloca los valores a, b y c en memoria y los inicializa, inicializa la matriz con 1.0 en cada posición. en **matmulblks** inicializa la vector c y genera un loop de 3 for en el cual se ejecutara el modulo **blkmul** el cual es utilizado para realizar la multiplicación del vector a y b en el vector c.
    - En temas de ejecución la función mmblk.c logra ser mas rapida que la función matrices.c debido a la técnica de multiplicación por bloques, esto reduce la cantidad de accesos a memoria y reduce la complejidad al dividir las matrices en bloques mas pequeños y manejables. Ademas de no poseer ningun tipo de operación redundante, esto reduce considerablemente el tiempo de calculo necesario para poder completar la multiplicación de matrices.
    - El tamaño optimo variara segun la arquitectura de la CPU, el tamaño de la matriz y la jerarquia de memoria del sistema de computo.
    - El tamaño de bloque optimo dependera de varios factores, estos son nuevamente la arquitectura de la CPU, la estructura del algoritmo, el tamaño de la matriz, el grado de paralelismo, la latencia de memoria y el ancho de banda. Para poder determinar cual es el tamaño optimo se requerira de pruebas empiricas para encontrar la configuración adecuada para lograr el mejor rendimiento segun el hardware y el problema a resolver en este caso la multiplicación de vectores...
    
