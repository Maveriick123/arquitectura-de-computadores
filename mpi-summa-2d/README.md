# Algoritmo SUMMA con MPI: distribución bidimensional

Implementación del algoritmo **SUMMA** (*Scalable Universal Matrix Multiplication Algorithm*) para el producto **C = α · A · B** con **MPI**. A diferencia de la [versión unidimensional](../mpi-producto-matrices-1d), aquí las tres matrices se reparten en bloques sobre una **malla cuadrada de procesos**, de forma que ningún proceso necesita una copia completa de B y la memoria por proceso se reduce considerablemente.

## Funcionamiento

Con *p* procesos organizados en una malla de √p × √p, cada proceso tiene un bloque de A, uno de B y uno de C. El algoritmo realiza √p iteraciones; en cada una:

1. El proceso de la columna correspondiente difunde su bloque de A **a lo largo de su fila** de la malla.
2. El proceso de la fila correspondiente difunde su bloque de B **a lo largo de su columna**.
3. Cada proceso multiplica los dos bloques recibidos y acumula el resultado en su bloque de C.

## Aspectos destacados

- **Tipos de datos derivados para la distribución 2D.** Un bloque de una matriz almacenada por filas no ocupa memoria contigua. Lo describo con `MPI_Type_vector` y ajusto su extensión con `MPI_Type_create_resized`, lo que permite repartir A y B y recoger C con una sola llamada a `MPI_Scatterv`/`MPI_Gatherv`, sin copias intermedias.
- **Comunicadores de fila y de columna.** Todas las difusiones del bucle principal utilizan comunicadores propios. He implementado dos alternativas seleccionables: una **topología virtual cartesiana** (`MPI_Cart_create` + `MPI_Cart_sub`) y una partición explícita con `MPI_Comm_split`.
- **Empaquetado de parámetros.** Las dimensiones y α, que son de tipos distintos, se envían en un único mensaje con `MPI_Pack`/`MPI_Unpack` en lugar de varias difusiones.
- **Gestión de memoria:** las matrices completas solo existen en el proceso 0, y todos los tipos derivados y comunicadores se liberan en cuanto dejan de usarse.
- Verificación automática del resultado frente a la versión secuencial.

## Restricciones

El número de procesos debe ser un cuadrado perfecto y *m*, *n* y *k* deben ser múltiplos de √p.

## Compilación y ejecución

```bash
mpicc -Wall summa.c -o summa -lm
mpirun -np 4 ./summa <m> <n> <k> <alfa>
# ejemplo: mpirun -np 16 --oversubscribe ./summa 8 4 8 1
```

## Archivos

```
summa.c              Implementación
docs/enunciado.pdf   Enunciado de la práctica
```
