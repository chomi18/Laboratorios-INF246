# Tarea-1-SO

### Integrantes:

Tomás González, ROL: 202273609-7

David Kripper, ROL: 202273521-K

### Instruciones de uso

Para utilizar el código se tiene que colocar lo siguiente en la terminal:

```
make

./main
```

Si se quiera limpiar los ejecutables que deja main.cpp se puede utilizar lo siguiente:

```
make

make clean
```

### Explicación del código

Las funciones creadas fueron:

- **leer_archivo:** la función lee el archivo que se el entregue y guarda su contenido en la variable "contenido".
- **obtener_data:** esta función me obtiene el tipo de evaluación que es, así como también su semestre y su número o nombre.
- **semestre_valido:** esta función me valida si el semestre es válido según el formato o según el supuesto de que tiene que pertenecer a 2025 o anteriores.
- **esta_en:** esta función me sirve para saber si un certamen o control tiene un número válido.
- **organizar_carpetas:** esta función me crea la carpeta OUTPUT, así como también las carpetas de controles, certamenes, tareas y corruptos y organiza los archivos según sea el caso.


### Supuestos utilizados

- Se asume que se tiene instalado las librerías **fstream**, **iostream**, **vector**, **string**, **filesystem**, **sstream** y **regex**.
- No se admiten nombres de tareas con **tíldes**. Es decir, que el nombre de las tareas pertenecen al alfabeto ingles.
- Se asume que se tiene una carpeta llamada "General" que contiene todos los archivos .txt que se quieren ordenar en la carpeta OUTPUT. La carpeta "General" debe estar en la misma carpeta en la que se encuentra el archivo "main.cpp". Luego de la ejecución del programa la carpeta "General" quedará vacía.
- Se asumió que las tareas, certamenes y controles solo pueden pertenecer al período 2025-2 o anteriores.
