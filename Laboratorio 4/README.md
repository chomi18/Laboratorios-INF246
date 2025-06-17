# Laboratorio-3-SO

### Integrantes:

Tomás González, ROL: 202273609-7, Paralelo: 200

David Kripper, ROL: 202273521-K, Paralelo: 200

### Instruciones de uso

Para utilizar el código se tiene que tener instalado Python3 y estar en el mismo directorio en el que se encuentra el archivo "LAB4_Kripper_Gonzalez.py". Una vez dentro del directorio, en la terminal se tiene que colocar:

```
python3 LAB4_Kripper_Gonzalez.py  
```
Esto correrá el script de python e irá generando cada 10 segundos los archivos de rondas.

Para el BONUS funciona de la misma manera. Dentro de la carpeta con dicho nombre, se encontrará otro makefile que compilará y correrá también ambos programas para el BONUS.

Si se quiere eliminar el ejecutable que deja la compilación se puede utilizar lo siguiente:
```
make clear
```

### Explicación del código de Python

Se creó la siguiente clase:

- **Celula:** la clase representa a cada célula como una hebra independiente.

Se crearon los siguiente métodos:

- **__init__:** inicializa el estado inicial de la célula. Además crea el historial.
- **marcarInfectada:** cambia la célula a "Alienígena" y registra el evento en el historial.
- **combatir:** Ajusta el combate según lo pedido en la tarea. Igualmente actualiza los historiales según sea el caso. 
- **run:** "vida" de cada célula. Lo que hace es ejecutar el combate por pareja, y luego espera hasta la siguiente ronda para continuar.

Las funciones creadas fueron:

- **escribirAislamiento:** crea "aislamiento.txt" con el estado inicial de cada célula.
- **escribirRonda:** genera "ronda_X.txt" mostrando el último historial de cada célula tras completar una ronda.
- **escribirDiagnosticoFinal:** crea "diagnostico_final.txt" indicando el estado final de cada célula y la ronda en la cual se infectó.
- **emparejar:** asigna las parejas aleatorias antes de cada ronda.
- **main:** ejecuta la simulación completa de la tarea. Crea las hebras, realiza las rondas y escribe los resultados.

### Explicación del código de Python para el BONUS

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

* Se asume que se tienen instaladas las librerías utilizadas.
* Se asume que se tiene instalado Python3.