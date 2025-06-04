# Laboratorio-3-SO

### Integrantes:

Tomás González, ROL: 202273609-7, Paralelo: 200

David Kripper, ROL: 202273521-K, Paralelo: 200

### Instruciones de uso

Para utilizar el código se tiene que colocar lo siguiente en la terminal, estando en el mismo espacio en donde se encuentra el makefile:

```
make   

make run    
```

Si se quiere eliminar el ejecutable que deja la compilación se puede utilizar lo siguiente:
```
make clear
```

### Explicación del código de C++

Las funciones creadas fueron:

- **leerMatriz:** lee las filas x columnas y arma una matriz utilizando vectores.
- **multiplicarFork:** multiplica 2 matrices utilizando fork() por fila. Cada hijo calcula su fila, la envía por un pipe, y el padre construye la matriz resultante.
- **procesarArchivos:** abre un archivo .txt que lee las 2 matrices. Mide el tiempo de la multiplicación de las matrices y escribe la matriz y valores resultantes en el archivo de salida.

### Explicación del código de C++ para el BONUS

Las funciones creadas fueron:

- **leerMatriz:** lee las filas x columnas y arma una matriz utilizando vectores.
- **leerTodasLasMatrices:** lee la primera linea n del archivo, para luego leer las n matrices. Devuelve un vector con todas las matrices.
- **MatricesCompatibles:** comprueba que cada par de matrices consecutivas pueda multiplicarse.
- **multiplicarFork:** multiplica las matrices utilizando fork() por fila. Cada hijo calcula su fila, la envía por un pipe, y el padre construye la matriz resultante.
- **multiplicarCadena:** encadena el producto de todas las matrices del vector llamando repetidamente a multiplicarFork.
- **MatrizEsSimetrica:** devuelve true si la matriz es simetrica. False en caso contrario.
- **procesarArchivos:** abre un archivo .txt que lee las matrices. Mide el tiempo de la multiplicación de las matrices y escribe la matriz y valores resultantes en el archivo de salida.

### Explicación del código de Java

### Supuestos utilizados

* Se asume que los archivos vienen en el formato del enunciado de la tarea.
* Se asume que se tienen instaladas las librerías utilizadas.
* Se asume que los .txt utilizados no están vacios.
