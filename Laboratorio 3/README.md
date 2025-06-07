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
Esto compilará y correrá ambos programas, el de Forks & Pipes y el de Threads. 
Para el BONUS funciona de la misma manera. Dentro de la carpeta con dicho nombre, se encontrará otro makefile que compilará y correrá también ambos programas para el BONUS.

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

Las funciones creadas fueron:

- **leerArchivo:** Lee el archivo donde se encuentran las amtrices a multiplicar y almacena los datos y valores en las variables correspondientes.
- **multiplicarFila:** Hace la multiplicación para una fila de la matriz resultante.
- **escribirResultadoEnArchivo:** Escribe la matriz resultante de la multiplicación en el archivo de salida. Escribe además el archivo correspondiente a la multiplicación, así como el tiempo que se demoró en realizar la multiplicación.
- **escribirErrorEnArchivo:** En caso de que se encuentre un error, lo escribe en el archivo de salida.

### Explicación del código en Java para el BONUS

Las funciones creadas fueron:

- **multiplicarMatrices:** Se encarga de realizar la multiplicación de las matrices del archivo utilizando hebras. Retorna la matriz resultante de dicha multiplicación.
- **multiplicarFila:** Hace la multiplicación para una fila de la matriz resultante.
- **esSimetrica:** Se encarga de hacer la evaluación de si una matriz es simétrica. Retorna _true_ o _false_ según sea el caso.

### Supuestos utilizados

* Se asume que los archivos vienen en el formato del enunciado de la tarea.
* Se asume que se tienen instaladas las librerías utilizadas.
* Se asume que los .txt utilizados no están vacios.
* Se asume que las carpetas donde se encuentran las matrices a multiplicar estarán en la ubicación junto a los programas o que se escribirá la dirección de dicha carpeta en las variables correspondietnes dentro de los programas.
* Ambos programas (C++ y Java para normal y BONUS) escriben los resultados de las matrices y la información correspondiente en un solo archivo de salida, resultando en 4 en total. (1 para C++ normal y otro para BONUS y 1 para Java normal y otro para BONUS).
