# Producto de matrices con MPI: distribución unidimensional

Implementación paralela del producto de matrices **C = α · A · B** mediante paso de mensajes con **MPI**, repartiendo el trabajo entre procesos con una distribución por filas.

## Planteamiento

El proceso 0 lee las dimensiones (*m*, *n*, *k*) y el factor de escala α, y genera las matrices completas. A partir de ahí:

1. **Difusión de parámetros** a todos los procesos con `MPI_Bcast`.
2. **Replicación de B** en todos los procesos, ya que cada uno la necesita entera para calcular sus filas de C.
3. **Distribución de A por bloques de filas** con `MPI_Scatterv`. Para que el programa funcione con **cualquier tamaño de matriz y cualquier número de procesos**, cuando el número de filas no es divisible entre los procesos el resto se reparte de una en una entre los primeros, de forma que la carga queda equilibrada con una diferencia máxima de una fila.
4. **Cómputo local**: cada proceso multiplica su bloque de A por B.
5. **Recolección de C** en el proceso 0 con `MPI_Gatherv`, usando los mismos desplazamientos calculados para la distribución.

Al terminar, el proceso 0 repite el cálculo de forma secuencial y compara ambos resultados elemento a elemento para verificar la corrección.

## Aspectos destacados

- **Todas las comunicaciones son operaciones colectivas** (sin envíos punto a punto), lo que simplifica el código y deja la optimización de las comunicaciones en manos de la implementación de MPI.
- Distribución en bloques con reparto equilibrado del resto, calculando `sendcounts` y `displs` para las versiones vectoriales de las colectivas.
- La memoria de las matrices completas solo se reserva en el proceso raíz.

## Compilación y ejecución

```bash
mpicc -Wall producto_matrices_1d.c -o producto_matrices_1d
mpirun -np 4 ./producto_matrices_1d <m> <n> <k> <alfa>
# ejemplo: mpirun -np 7 --oversubscribe ./producto_matrices_1d 6 5 4 2.2
```

## Archivos

```
producto_matrices_1d.c   Implementación
docs/enunciado.pdf       Enunciado de la práctica
```
