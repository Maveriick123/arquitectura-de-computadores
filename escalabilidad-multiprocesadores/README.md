# Escalabilidad en multiprocesadores: del *snooping* en UMA al directorio en CC-NUMA

**Memoria:** [Escalabilidad_en_Multiprocesadores.pdf](Escalabilidad_en_Multiprocesadores.pdf)

Estudio teórico sobre los límites de escalabilidad de los sistemas multiprocesador de memoria compartida, centrado en el problema de la **coherencia caché** y en cómo los protocolos que la mantienen han tenido que evolucionar a medida que crece el número de procesadores.

## Motivación

El estancamiento del rendimiento de los uniprocesadores, al agotarse las mejoras basadas en el paralelismo a nivel de instrucción, llevó a la industria a explotar el paralelismo a nivel de hilo mediante arquitecturas multiprocesador. Su gran ventaja es un espacio de direcciones compartido que facilita la programación, pero el uso de cachés privadas introduce el riesgo de que distintos núcleos vean valores diferentes para un mismo dato. Garantizar que esto no ocurra, sin que el coste de hacerlo frene el crecimiento del sistema, es el eje del trabajo.

## Contenido

**Coherencia y consistencia.** Distingo entre ambos conceptos: la coherencia asegura que todos los procesadores vean un orden único de escrituras sobre una misma posición, mientras que la consistencia define cuándo se hacen visibles las escrituras sobre posiciones distintas. Lo ilustro con el clásico ejemplo de Hennessy y Patterson, analizando las ejecuciones que permite la consistencia secuencial y por qué los sistemas reales recurren a modelos relajados.

**Protocolos de *snooping* en UMA.** Describo los protocolos de invalidación basados en bus (familia MSI, MESI, MOESI) y, a partir del análisis cuantitativo de Hennessy y Patterson, muestro que el límite de escalabilidad no está solo en el ancho de banda del bus, sino en la capacidad de los controladores de caché para procesar todas las peticiones de espionaje.

**Directorios en CC-NUMA.** Al distribuir físicamente la memoria, el *broadcast* deja de ser viable y se sustituye por un directorio que registra qué cachés tienen copia de cada bloque. Comparo distintas organizaciones:
- **Directorio de mapeado completo**, sencillo pero con un coste de memoria que crece de forma cuadrática con el número de procesadores.
- **Directorio limitado**, que sustituye los bits de presencia por un número fijo de punteros y reduce el coste a O(N·log N).
- **Directorios distribuidos y encadenados** (como el estándar SCI), que reparten el seguimiento de las copias entre las propias cachés.

## Conclusiones

La elección del protocolo depende fundamentalmente de la escala del sistema. El *snooping* es eficiente con pocos núcleos, pero su naturaleza centralizada lo hace inviable a gran escala; los directorios permiten escalar a cambio de mayor latencia y de un coste de almacenamiento que obliga a buscar representaciones más compactas.

## Bibliografía principal

- J. L. Hennessy y D. A. Patterson, *Computer Architecture: A Quantitative Approach*, 6.ª ed.
- Bibliografía específica sobre protocolos de coherencia y directorios (ver memoria).
