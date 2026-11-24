# Arquitectura de Computadores

Trabajos realizados en la asignatura **Arquitectura de Computadores** (Grado en Ingeniería Informática, Universidade da Coruña, curso 2025/2026).

El repositorio cubre el rendimiento a distintos niveles: desde el aprovechamiento de un único núcleo (vectorización, jerarquía de memoria) hasta la programación paralela en memoria distribuida con MPI, pasando por un estudio teórico sobre la escalabilidad de los multiprocesadores de memoria compartida.

## Contenido

| Trabajo | Tipo | Temas |
|---|---|---|
| [Escalabilidad en multiprocesadores](escalabilidad-multiprocesadores) | Estudio teórico | Coherencia y consistencia caché, *snooping*, protocolos MSI/MESI, UMA frente a CC-NUMA, directorios |
| [Optimización de la multiplicación matriz-vector](optimizacion-matvec) | Análisis de rendimiento | Roofline, TMA, VTune, `perf`, vectorización AVX2, intrínsecas de Intel |

Las prácticas de programación paralela con MPI de esta asignatura están en repositorios propios:

- [**summa-mpi**](https://github.com/Maveriick123/summa-mpi): multiplicación de matrices distribuida con el algoritmo SUMMA sobre una malla 2D de procesos.
- [**producto-matrices-mpi**](https://github.com/Maveriick123/producto-matrices-mpi): producto de matrices con distribución unidimensional y operaciones colectivas.

## Tecnologías y herramientas

- **Lenguaje:** C con MPI (Open MPI) e intrínsecas AVX2/FMA
- **Hardware:** Intel Core i7-1260P (Alder Lake, núcleo P Golden Cove)
- **Análisis:** Intel VTune Profiler, Linux `perf`, Empirical Roofline Toolkit (ERT), PolyBench
- **Sistema:** `cpupower`, `numactl`, Huge Pages
- **Documentación:** LaTeX
