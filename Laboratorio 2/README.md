# Laboratorio-2-SO

### Integrantes:

Tomás González, ROL: 202273609-7, Paralelo: 200

David Kripper, ROL: 202273521-K, Paralelo: 200

### Instruciones de uso

Para utilizar el código se tiene que colocar lo siguiente en la terminal:

```
make   

make run    
```
Si se quiere ejecutar el código con valgrind se debe usar lo siguiente:
```
make valgrind
```
Si se quiere eliminar el ejecutable que deja la compilación se puede utilizar lo siguiente:
```
make clear
```

### Explicación del código

Las funciones creadas fueron:

- **iniciarMazo:** Inicia el mazo. Un arreglo donde cada posición de este almacena la figura, la pinta y el valor de la carta.
- **revolverCaras:** Revuelve el mazo para que el arreglo no esté en orden a la hora de repartir cartas.
- **pedirCarta:** Siguiendo las indicaciones del enunciado, determina si el jugador pide carta o se planta planta en la ronda.
- **repartirCarta:** Retorna la carta destinada al jugador o al crupier según corresponda. Recorre el arreglo del mazo, aprovechando que están revueltas. Reparte las cartas que no han sido repartidas
- **JugadirJuega:** Se encarga de llevar el flujo de cada jugador en la ronda. Recibe las cartas principales del juagor, espera al turno del crupier, toma decisiones y las muestra por pantalla, etc.
- **CrupierJuega:** Lleva el flujo del crupier en cada ronda. Recibe las cartas principales del crupier, mantiene una carta oculta, espera que los jugadores terminen su turno, deja al usuario tomar control de la decisión que se toma, etc.
- **determinarGanadorRonda:** Reparte los puntos de la victoria de la ronda. Si el jugador gana al crupier les reparte 1 punto. El crupier recibe puntos según se indicó en el enunciado de la tarea y los supuestos.
- **mostrarEstado:** Muestra por pantalla los puntos por las rondas ganadas, hasta el momento en que se solicita. Si se llama al final de la ronda 2, se imprimirán los puntos que tienen los jugadores y el crupier hasta ese momento.
- **mostrarGanadorFinal:** Obtiene a aquel que acumuló más puntos durante todas las rondas. En caso de empate, los obtiene a todos e imprime por pantalla a cada uno de ellos, indicando que terminó en empate.


### Supuestos utilizados

- Se cree el jugador va a ingresar correctamente todos los datos. Un valor entero pequeño para la cantidad de rondas a jugar y el caracter C para pedir carta y P para plantarse (en el turno del crupier).
- Al asignar "puntos de victoria de ronda" a los jugadores, se entiende por enunciado que al decir, "Cada victoria vale +1 punto para cada jugador...", se habla de haber ganado explícitamente. No se otorgarán "puntos de victoria de ronda" por empatar en "puntos de ronda" y se imrpimirá por pantalla que el jugador "no perdió" con el crupier. En caso excepcional a lo anterior, se consideró que si el crupier o jugador tienen ambos 21 "puntos de ronda", el ganador se determina por el criterio de aquel que tenga menos cartas en mano.
- Si el jugador se pasa de 21 puntos, perderá automáticamente, sin importar los puntos del crupier.
- Al asignar los "puntos de victoria de ronda" al crupier, se entiende por enunciado que al decir, "...pero el crupier recibe solo +1 punto por ronda si gana la mayoría de duelos...", se habla de haberle ganado a tres jugadores en la ronda. Si el crupier le gana a 2 jugadores, empata con otro y pierde con el ultimo jugador, no se le otorgarán puntos puesto que solo le ganó a dos jugadores (no cuenta como la mayoría).
- Si el crupier (usuario) se encuentra tomando decisiones, y elige pedir una carta pero se pasa de los puntos, el turno del crupier terminará atuomáticamente e imprimirá por pantalla el resultado de la ronda y continuará con el flujo de las demás rondas.

