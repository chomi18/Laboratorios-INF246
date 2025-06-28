# Laboratorio-4-SO

### Integrantes:

Tomás González, ROL: 202273609-7, Paralelo: 200

David Kripper, ROL: 202273521-K, Paralelo: 200

### Instruciones de uso

Para utilizar el código se tiene que tener instalado Python3 y estar en el mismo directorio en el que se encuentra el archivo "LAB4_Kripper_Gonzalez.py". Una vez dentro del directorio, en la terminal se tiene que colocar:

```
make
```
Esto correrá el script de python  y preguntará si se quiere ejecutar el código con o sin bonus, e irá generando cada 10 segundos los archivos correspondientes de rondas. También sirve ingresar:

```
make run
```

Si se quiere eliminar los ejecutables que deja la compilación se puede utilizar lo siguiente:
```
make clear
```

### Explicación del código de Python

Se crearon las siguientes clases:

- **Celula:** la clase representa a cada célula como una hebra independiente.
- **Anticuerpo:** la clase representa a la hebra del anticuerpo.

Se crearon los siguiente métodos para la clase **Célula**:

- **__init__:** inicializa el estado inicial de la célula. Además crea el historial.
- **marcarInfectada:** cambia la célula a "Alienígena" y registra el evento en el historial.
- **curar:** Función para el bonus: Se cura la célula si es que se encuentra con el anticuerpo y fue infectada hace máximo dos rondas.
- **combatir:** Ajusta el combate según lo pedido en la tarea. Igualmente actualiza los historiales según sea el caso. 
- **run:** "vida" de cada célula. Lo que hace es ejecutar el combate por pareja, y luego espera hasta la siguiente ronda para continuar.

Se crearon los siguiente métodos para la clase **Anticuerpo**:
- **__init__:** inicializa el estado inicial del anticuerpo. Le carga todas las células del programa junto con sus características.
- **run:** espera una señal estrablecida en el main que avisa que el anticuerpo actúe despues de los enfentamientos en cada ronda.
- **acciones_ronda:** reliza las acciones correspondientes según lo especificado en el enunciado haciéndo máximo 4 acciones. Crea el archivo "acciones_anticuerpo.txt" y escribe en él las acciones del anticuerpo por cada ronda.

Las funciones creadas fueron:

- **escribirAislamiento:** crea "aislamiento.txt" con el estado inicial de cada célula.
- **escribirRonda:** genera "ronda_X.txt" mostrando el último historial de cada célula tras completar una ronda.
- **escribirDiagnosticoFinal:** crea "diagnostico_final.txt" indicando el estado final de cada célula y la ronda en la cual se infectó.
- **emparejar:** asigna las parejas aleatorias antes de cada ronda.
- **main:** ejecuta la simulación completa de la tarea. Crea las hebras, realiza las rondas y escribe los resultados.

### Supuestos utilizados

* Se asume que se tienen instaladas las librerías utilizadas.
* Se asume que se tiene instalado Python3.
* Se asume que al preguntar por iniciar el bonus se va a ingresar correctamente el input solicitado (s/n).
