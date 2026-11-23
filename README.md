# Arquitectura de Computadores

Trabajos realizados en la asignatura **Arquitectura de Computadores** (Grado en Ingeniería Informática, Universidade da Coruña, curso 2025/2026).

El repositorio cubre el rendimiento a distintos niveles: desde el aprovechamiento de un único núcleo (vectorización, jerarquía de memoria) hasta la programación paralela en memoria distribuida con MPI, pasando por un estudio teórico sobre la escalabilidad de los multiprocesadores de memoria compartida.

## Contenido

| Trabajo | Tipo | Temas |
|---|---|---|
| [Producto de matrices con MPI (1D)](mpi-producto-matrices-1d) | Programación paralela | Operaciones colectivas, distribución por bloques de filas con reparto equilibrado |
| [Algoritmo SUMMA con MPI (2D)](mpi-summa-2d) | Programación paralela | Malla de procesos, tipos de datos derivados, topologías virtuales, comunicadores |
| [Escalabilidad en multiprocesadores](escalabilidad-multiprocesadores) | Estudio teórico | Coherencia y consistencia caché, *snooping*, protocolos MSI/MESI, UMA frente a CC-NUMA, directorios |
| [Optimización de la multiplicación matriz-vector](optimizacion-matvec) | Análisis de rendimiento | Roofline, TMA, VTune, `perf`, vectorización AVX2, intrínsecas de Intel |

## Tecnologías y herramientas

- **Lenguaje:** C con MPI (Open MPI) e intrínsecas AVX2/FMA
- **Hardware:** Intel Core i7-1260P (Alder Lake, núcleo P Golden Cove)
- **Análisis:** Intel VTune Profiler, Linux `perf`, Empirical Roofline Toolkit (ERT), PolyBench
- **Sistema:** `cpupower`, `numactl`, Huge Pages
- **Documentación:** LaTeX
