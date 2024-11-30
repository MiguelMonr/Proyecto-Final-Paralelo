# PROYECTO DE CLAUSURA: Bolsa de Palabras con MPI para Análisis de Texto

Este proyecto consiste en implementar una matriz de Bolsa de Palabras utilizando procesamiento paralelo con MPI y una versión serial para comparar tiempos de ejecución y calcular el speed-up.

Creado por: Miguel Monreal y Lucía Varela 



## Descripción y Objetivos del Proyecto

La tarea principal es procesar un conjunto de libros (6 en total), extraer su vocabulario y generar una matriz de Bolsa de Palabras. Se compararán los tiempos de ejecución entre una implementación paralela y una serial para evaluar la eficiencia de la paralelización. 
El objetivo del proyecto es poder comparar la version serial (procesará los libros de manera secuencial para obtener la misma salida) y la version paralela (distribuye la carga de trabajo entre procesos, asignando un libro a cada uno de estos mismos) del proyecto y observar como es que funciona el speed-up. 

### Funcionalidades

1. Procesar automáticamente una lista de 6 libros.
2. Generar una matriz de Bolsa de Palabras en formato CSV.
3. Comparar tiempos de ejecución entre las versiones paralela y serial.
4. Generar gráficos de speed-up para evaluar el rendimiento.

## Requerimientos del Sistema
- C/C++ (implementación del proyecto).
- Python (solo para la extracción inicial del vocabulario y su tamaño).
- MPI (Message Passing Interface) para la paralelización.
- Herramientas gráficas (opcional) para generar gráficos de speed-up.

## Requisitos de Entrada y Salida

### Entrada

1. **Archivos:** Lista de nombres de archivos de texto (6 libros).
2. **Vocabulario:** Archivo que contiene el vocabulario y su tamaño.
3. **Parámetros:** Número de procesos a utilizar (igual al número de libros).
### Salida

1. Matriz de Bolsa de Palabras en formato CSV.
2. Tiempos de ejecución para las versiones serial y paralela.
3. Gráfica de speed-up.



