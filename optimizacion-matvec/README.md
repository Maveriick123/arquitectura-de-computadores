# Optimización de la multiplicación matriz-vector: Roofline y TMA

**Memoria:** [Optimizacion_multiplicacion_matriz_vector_Roofline_TMA.pdf](Optimizacion_multiplicacion_matriz_vector_Roofline_TMA.pdf)

Trabajo de análisis y optimización de rendimiento de un *kernel* de multiplicación matriz-vector en precisión simple (matriz de 10 000 × 10 000, unos 400 MB) sobre un único núcleo de rendimiento de un **Intel Core i7-1260P (Alder Lake)**. El objetivo no era solo acelerar el código, sino entender en cada paso qué recurso del procesador lo estaba limitando y por qué.

**Resultado:** de **1,02 GFLOPS** en la versión escalar a **10,38 GFLOPS** en la versión final, una mejora de unas **10×**.

## Metodología

Para que las medidas fueran reproducibles fijé la frecuencia del núcleo a la nominal (2,1 GHz) con `cpupower`, aislé la ejecución en una única CPU lógica con `numactl` para evitar interferencias del *Hyper-Threading* y activé las *Huge Pages* para eliminar prácticamente los fallos de TLB.

- **Modelo Roofline.** Construí el Roofline empírico del núcleo con la herramienta ERT y lo contrasté con el pico teórico calculado a partir de la microarquitectura (3 unidades FMA, AVX2 de 256 bits). El análisis de intensidad aritmética (unos 0,25 FLOP/byte) confirma que el *kernel* está limitado por memoria.
- **Top-Down Microarchitecture Analysis (TMA).** En cada versión identifiqué el cuello de botella dominante (*Core Bound*, *Memory Bound*, utilización de puertos, ocupación de la *Super Queue*…) con **Intel VTune** y lo validé con contadores hardware leídos directamente con `perf`.
- **Ensamblador.** Revisé el código generado por el compilador en cada etapa para relacionar las métricas con las instrucciones concretas.

## Evolución de las optimizaciones

| Versión | GFLOPS | Observación principal |
|---|---|---|
| `-O2 -fno-tree-vectorize` | 1,02 | Código escalar; dependencias entre FMAs consecutivas y latencia de L1 |
| `-O2 -ftree-vectorize` | 1,96 | Vectoriza las multiplicaciones, pero el estricto cumplimiento del IEEE 754 obliga a reducir las sumas en orden y satura un único puerto de *shuffle* |
| `+ -ffast-math` | 7,72 | Relajar la asociatividad permite FMAs AVX2 completas (16 FLOP por instrucción) |
| `+ -funroll-loops -fvariable-expansion-in-unroller` | 8,32 | Varios acumuladores independientes permiten solapar FMAs |
| Intrínsecas AVX2 + desenrollado de 2 filas | 9,59 | Reutilización del vector `x` entre filas |
| **Intrínsecas AVX2 + desenrollado de 4 filas** | **10,38** | Accesos a `x` reducidos a la cuarta parte |
| *Tiling* (descartado) | 9,48 | Sin reutilización posible de la matriz; solo añade sobrecarga |

## Conclusiones

Una vez liberada la capacidad de cálculo con AVX2 y FMA, el procesador consume datos más rápido de lo que la jerarquía de memoria puede servirlos, y el cuello de botella se desplaza hacia el ancho de banda de la DRAM, tal como anticipaba el Roofline. La mejora final se obtiene reduciendo el tráfico de memoria sobre el vector `x`. También comprobé, y lo confirmé experimentalmente, que el *tiling* no aporta nada en este caso, porque cada elemento de la matriz se lee una sola vez y no hay localidad temporal que aprovechar.

## Código

```
src/matvec_avx2.c     Versión final: intrínsecas AVX2/FMA con desenrollado de 4 filas
src/polybench.c/.h    PolyBench/C, utilizado para medir tiempo y GFLOPS
src/perf_metrics.sh   Lectura de contadores hardware (Top-Down, FP, puertos, caché) con perf
src/Makefile
docs/enunciado.pdf    Enunciado de la práctica
```

Compilación y ejecución (requiere un procesador con AVX2 y FMA):

```bash
cd src
make
./perf_metrics.sh
```
