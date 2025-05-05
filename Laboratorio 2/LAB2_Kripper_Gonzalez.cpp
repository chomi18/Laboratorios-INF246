#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>

using namespace std;

#define NUM_RONDAS 5
#define MAX_PUNTOS 21
#define NUM_JUGADORES 4
#define NUM_CARTAS 52

struct Carta
{
    int valor;
    char pinta;
    const char* figura;
};

struct Jugador
{
    int puntos;
    int puntos_ronda;
    bool plantado;
    bool perdio;
    pid_t pid;
    Carta cartas_jugador[12];
    int num_cartas;
};

struct MemoriaCompartida
{
    Carta mazo[NUM_CARTAS];
    Jugador jugadores[NUM_JUGADORES];
    Jugador crupier;
    int cartas_repartidas;
    int turno_actual;
    bool ronda_terminada;
    int rondas_jugadas;
    bool crupier_recibe_cartas;
};

void iniciarMazo(MemoriaCompartida *mem); 
/* void iniciarMazo: Esta función se encarga de iniciar el mazo con el que se va a jugar, creando un arreglo de tipo "Carta", donde cada indice almacena la figura de la carta (A, 2, 3, etc), la pinta
de la carta (Pica (P), Diamantes(D), Corazones(C) y Tréboles(T)). Además de el valor correspondiente de cada carta (A=11 [El valor de A=1 se ajusta en otra función], 2=10 ... 10=10,
J=10, Q=10, K=10)
*/
void revolverCartas(MemoriaCompartida *mem);
/*void revolverCartas: Se encarga de revolver el mazo, puesto que como el mazo es creado en orden, se revuelve para posteriormente repartir las cartas recorriendo el arreglo que las contiene en orden.
Reinicia la variable, del struct MemoriaCompartida, cartas_repartidas, puesto que gracias a esta variable es que se reparten cartas como si fuera un mazo normal*/
bool pedirCarta(int puntos);
/*bool pedirCarta: Esta función se encarga de determinar si el jugador pedirá una carta o se plantará, tal como especifica en el enunciado de la tarea*/
Carta repartirCarta(MemoriaCompartida *mem);
/*Carta repartirCarta: Esta función se encarga de retornar una carta del mazo, la cual será asignada posteriormente a un Jugador o al Crupier*/
void JugadorJuega(MemoriaCompartida *mem);
/*void JugadorJuega: Esta función hace todo lo que tiene que realizar el jugador en cada ronda. Recibe dos cartas y las imprime por pantalla con la figura, la pinta de cada una y los puntos que acumulan las 2. Luego, gracias a la
función pedirCarta, el jugador decide si pedir otra Carta o si Plantarse. Si decide sacar carta, se imprime por pantalla la carta obtenida y además los puntos actualziados con la carta obtenida. Si decide palantarse, se imprime
por pantalla la decisión y también los puntos totales que obtiene. Si el jugador se pasa de los 21 puntos, se imprime por pantalla que perdió junto con los puntos que obtuvo al pasarse. Si el jugador se pasa de los puntos pero 
tiene un As en la mano, se ajustan los puntos de la carta. Esto se hace por cada As que tenga u obtenga en el futuro cada vez que pide y se pasa de los puntos. 
*/
void CrupierJuega(MemoriaCompartida *mem);
/*void CrupierJuega: Similar a JugadorJuega. Hace todo lo que tiene que ver con los turnos del Crupier en cada ronda. Obtiene sus dos cartas y se muestra por pantalla solamente una, la otra permanece oculta hasta que sea el momento
se revelarla. Una vez todos los jugadores hayan terminado sus turnos, se revela la segunda carta del Crupier, y el usuario empieza a tomar decisiones, usando (C) para pedir Carta y (P) para Plantarse. Se muestra por pantalla la
información de la Carta si decide pedir, o un resumen de su mano si decide plantarse o si pierde. Se ajustan igualmente los valores de los Ases.
*/
void determinarGanadorRonda(MemoriaCompartida *mem);
/* void determinarGanadorRonda: Esta función determina si el Jugador i (con i = 1...NUM_JUGADORES) ganó, perdió o empató con el crupier y reparte los puntos en caso de que haya ganado. Hace lo mismo con el Crupier, reparitendo
los puntos correspondientes según a cuantos Jugadores le haya ganado. 
*/
void mostrarEstado(MemoriaCompartida *mem);
/*void mostrarEstado: Esta función muestra un resumen de los puntos acumulados, por victorias de rondas por Jugadores y Crupier, hasta la ronda actual.
*/
void mostrarGanadorFinal(MemoriaCompartida *mem);
/*void mostrarGanadorFinal: Esta función determina quien fue el ganador del juego completo, eligiéndolo segun el que haya acumulado más punros a través de las rondas. En caso de empate, muestra a todos aquellos que hayan ganado
con la misma puntuación más alta.
*/

int main(){

    cout << "---------------------------------------------------------------------------------------------------------" << endl;
    cout << "                                       BIENVENIDO A BLACK-JACK USM                                       " << endl;
    cout << "---------------------------------------------------------------------------------------------------------\n" << endl;
    
    // Configurar memoria compartida
    int shmid = shmget(IPC_PRIVATE, sizeof(MemoriaCompartida), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    MemoriaCompartida *mem = (MemoriaCompartida *)shmat(shmid, NULL, 0);
    if (mem == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // Inicializar memoria compartida
    memset(mem, 0, sizeof(MemoriaCompartida));
    iniciarMazo(mem);
    revolverCartas(mem);

    // Configurar número de rondas
    int cantidad_rondas;
    cout << "Ingrese el número de rondas que se desea jugar: ";
    cin >> cantidad_rondas;
    cout << "\n";

    //Ciclo de las rondas
    while(mem->rondas_jugadas < cantidad_rondas){
        cout << "\n---------------------------------------------------------------------------------------------------------" << endl;
        cout << "                                               RONDA: " << mem->rondas_jugadas+1 << endl;
        cout << "---------------------------------------------------------------------------------------------------------" << endl;
        mem->crupier_recibe_cartas = false;
        //Se crean los jugadores
        for(int num_jugador = 0; num_jugador < NUM_JUGADORES; num_jugador++){
            pid_t pid = fork();
            if(pid == 0){
                mem->jugadores[num_jugador].pid = pid;
                JugadorJuega(num_jugador, mem);
                exit(0);
            } 
        }
        CrupierJuega(mem);
        determinarGanadorRonda(mem);
        sleep(2);
        mem->rondas_jugadas++;
        mostrarEstado(mem);
        revolverCartas(mem);
        sleep(2);
    }

    cout << "---------------------------------------------------------------------------------------------------------" << endl;
    cout << "                                              RESUMEN FINAL                                              " << endl;
    cout << "---------------------------------------------------------------------------------------------------------" << endl;

    mostrarGanadorFinal(mem);
    cout << endl;
    // Liberar memoria compartida
    shmdt(mem);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}

void iniciarMazo(MemoriaCompartida *mem){
    char pintas[] = {'C', 'D', 'T', 'P'};
    const char* figuras[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int valores[] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    int indice = 0;
    //Crea el mazo, con las cartas en orden
    for (int pinta = 0; pinta < 4; pinta++)
    {
        for (int figura = 0; figura < 13; figura++)
        {
            mem->mazo[indice].valor = valores[figura];
            mem->mazo[indice].pinta = pintas[pinta];
            mem->mazo[indice].figura = figuras[figura];
            mem->mazo[indice].repartida = false;
            indice++;
        }
        
    }
}

void revolverCartas(MemoriaCompartida *mem){
    //"Recoge" las cartas de los jugadores
    mem->cartas_repartidas = 0;
    srand(time(NULL));
    //Revuelve las cartas
    for (int i = 0; i < 52; i++)
    {
        int aux = rand() % 52;
        Carta temp = mem->mazo[i];
        mem->mazo[i] = mem->mazo[aux];
        mem->mazo[aux] = temp;
        mem->mazo[i].repartida = false;

    }
}

bool pedirCarta(int puntos){
    if(puntos <= 11) return true;
    if(puntos >= 19) return false;
    return rand() % 2 == 0;
}

Carta repartirCarta(MemoriaCompartida *mem){
    Carta carta = mem->mazo[mem->cartas_repartidas];
    mem->cartas_repartidas++; //Por cada carta repartida avanza en uno la posición del arreglo y simula que las cartas no están en el mazo, hasta que sean revueltas nuevamente
    return carta;
}

void JugadorJuega(int id, MemoriaCompartida *mem){
    usleep(100000*id); //Esperar para evitar colisiones con las impresiones
    //Setea los valores base de cada variable para que cada jugador empiece de "0" 
    mem->jugadores[id].perdio = false;
    mem->jugadores[id].plantado = false;
    mem->jugadores[id].num_cartas = 0;
    mem->jugadores[id].puntos_ronda = 0;
    //Carta 1
    mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas] = repartirCarta(mem);
    mem->jugadores[id].puntos_ronda += mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].valor;
    mem->jugadores[id].num_cartas++;
    //Carta 2
    mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas] = repartirCarta(mem);
    mem->jugadores[id].puntos_ronda += mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].valor;
    mem->jugadores[id].num_cartas++;
    //Imprime por pantalla la información de las dos cartas
    cout << "Jugador " << id+1 << " tiene las cartas: " << mem->jugadores[id].cartas_jugador[0].figura << "-" << mem->jugadores[id].cartas_jugador[0].pinta << 
    " y " << mem->jugadores[id].cartas_jugador[1].figura << "-" << mem->jugadores[id].cartas_jugador[1].pinta << " y tiene " << mem->jugadores[id].puntos_ronda << " puntos.\n";
    
    while(!mem->crupier_recibe_cartas){
        //El Jugador espera a que el crupier termine de recibir sus cartas e impirma por pantalla
    }
    usleep(100000*id);//Espera nuevamente para que no haya colisiones entre los jugadores

    int revisador_A = 0; //Guarda la posición de donde se encontraba la última A para no reajustar otra vez el valor del mismo As
    while(mem->jugadores[id].puntos_ronda < MAX_PUNTOS){
        if(pedirCarta(mem->jugadores[id].puntos_ronda)){
            //Pide carta segun las reglas y la función pedirCarta()
            mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas] = repartirCarta(mem);
            mem->jugadores[id].puntos_ronda += mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].valor;
            //Informa por pantalla la información de la carta obtenida
            cout << "El Jugador " << id+1 << " obtuvo la carta: " << mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].figura << 
            "-" << mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].pinta;
            mem->jugadores[id].num_cartas++;
            //Revisa si el Jugador tiene un As en su mano, si es así le resta 10 a los puntos de la mano (Ajusta su valor)
            for(int i = revisador_A; i < mem->jugadores[id].num_cartas; i++){
                if(strcmp(mem->jugadores[id].cartas_jugador[i].figura, "A") == 0 && mem->jugadores[id].puntos_ronda > MAX_PUNTOS){
                    mem->jugadores[id].puntos_ronda -= 10;
                    revisador_A = i+1;
                    break;
                }
            }
            cout << ". Puntos actuales: " << mem->jugadores[id].puntos_ronda << "\n";
        } else {
            break;
        }  
    }
    string estado; //Según el estado del jugador, se imprimirá una información u otra por pantalla.
    if(mem->jugadores[id].puntos_ronda > MAX_PUNTOS){
        estado = "perdió";
        mem->jugadores[id].perdio = true;
    } else {
        estado = "se planta";
        mem->jugadores[id].plantado = true;
    }
    //Se imprime el resultado del Jugador en la ronda, con las cartas obtenidas y los puntos obtenidos.
    cout << "El Jugador " << id+1 << " " << estado << ". Cartas: ";
    for(int i = 0; i < mem->jugadores[id].num_cartas; i++){
        if(i + 1 == mem->jugadores[id].num_cartas){
            cout << mem->jugadores[id].cartas_jugador[i].figura << "-" << mem->jugadores[id].cartas_jugador[i].pinta << ". ";
        } else {
            cout << mem->jugadores[id].cartas_jugador[i].figura << "-" << mem->jugadores[id].cartas_jugador[i].pinta << " / " ;
        }
    }
    cout << "Puntos: " << mem->jugadores[id].puntos_ronda << "\n";

    cout << endl;
    sleep(1);                                                                   
}

void CrupierJuega(MemoriaCompartida *mem){
    //Resetea los valores de las variables del Crupier para una ronda nueva
    mem->crupier.perdio = false;
    mem->crupier.plantado = false;
    mem->crupier.num_cartas = 0;
    mem->crupier.puntos_ronda = 0;
    sleep(1);
    cout << "\n ============================================= Turno del Crupier ============================================= \n" << endl;
    //Carta 1
    mem->crupier.cartas_jugador[mem->crupier.num_cartas] = repartirCarta(mem);
    mem->crupier.puntos_ronda += mem->crupier.cartas_jugador[mem->crupier.num_cartas].valor;
    mem->crupier.num_cartas++;
    //Carta 2
    mem->crupier.cartas_jugador[mem->crupier.num_cartas] = repartirCarta(mem);
    mem->crupier.puntos_ronda += mem->crupier.cartas_jugador[mem->crupier.num_cartas].valor;
    mem->crupier.num_cartas++;
    //Imprime las cartas del Crupier y mantiene una carta oculta
    cout << "Crupier tiene las cartas: " << mem->crupier.cartas_jugador[0].figura << "-" << mem->crupier.cartas_jugador[0].pinta << 
    " y [Carta boca abajo]" << endl;
    cout << endl;

    cout << " ============================================= Turno del Crupier =============================================\n" << endl;

    mem->crupier_recibe_cartas = true;

    for (int i = 0; i < NUM_JUGADORES; i++){
        waitpid(mem->jugadores[i].pid, NULL, 0); //Espera a que los Jugadores terminen de realizar sus acciones, si pedir carta o plantarse 
    }

    cout << " ============================================= Turno del Crupier =============================================" << endl;
    //Crupier revela su carta y se muestra la información de su mano
    cout << "Crupier da vuelta su carta. Sus cartas son: " << mem->crupier.cartas_jugador[0].figura << "-" << mem->crupier.cartas_jugador[0].pinta << 
    " y " << mem->crupier.cartas_jugador[1].figura << "-" << mem->crupier.cartas_jugador[1].pinta << " y tiene " << mem->crupier.puntos_ronda << " puntos.\n" << endl;
    if(mem->crupier.puntos_ronda == MAX_PUNTOS) cout << " ============================ Turno del Crupier: ============================\n" << endl;

    int revisador_A = 0;
    while(mem->crupier.puntos_ronda < MAX_PUNTOS){
        char decision;
        cout << "Ingrese su desición. '(C) para pedir Carta o (P) para Plantarse: '"; //Deja al usuario decidir la acción que se debe tomar
        cin >> decision;
        cout << endl;
        if(decision == 'C'){
            mem->crupier.cartas_jugador[mem->crupier.num_cartas] = repartirCarta(mem);
            mem->crupier.puntos_ronda += mem->crupier.cartas_jugador[mem->crupier.num_cartas].valor;
            //Informa por pantalla la información de la carta obtenida
            cout << "Carta obrenida: " << mem->crupier.cartas_jugador[mem->crupier.num_cartas].figura << 
            "-" << mem->crupier.cartas_jugador[mem->crupier.num_cartas].pinta;
            mem->crupier.num_cartas++;
            //Verifica si el Crupier se pasó de puntos y si la carta obtenida es un As, ajusta los puntos
            for(int i = revisador_A; i < mem->crupier.num_cartas; i++){
                if(strcmp(mem->crupier.cartas_jugador[i].figura, "A") == 0 && mem->crupier.puntos_ronda > MAX_PUNTOS){
                    mem->crupier.puntos_ronda -= 10;
                    revisador_A = i+1;
                    break;
                }
            }
            if(mem->crupier.puntos_ronda > MAX_PUNTOS){ //Si se pasa de los puntos, termina el ciclo de decision y no le pregunta por mas desiciones al usuario. Muestra por pantalla el resultado
                cout << "\nEl Crupier perdió. Cartas: ";
                mem->crupier.perdio = true;
                for(int i = 0; i < mem->crupier.num_cartas; i++){
                    if(i + 1 == mem->crupier.num_cartas){
                        cout << mem->crupier.cartas_jugador[i].figura << "-" << mem->crupier.cartas_jugador[i].pinta << ". ";
                    } else {
                        cout << mem->crupier.cartas_jugador[i].figura << "-" << mem->crupier.cartas_jugador[i].pinta << " / " ;
                    }
                }   
                cout << "Puntos: " << mem->crupier.puntos_ronda << "\n";
                cout << " ============================================= Turno del Crupier =============================================\n" << endl;
                break;
            } else {
                cout << ". Puntos actuales: " << mem->crupier.puntos_ronda << "\n";
            }
        } else if(decision == 'P') { //Muestra por pantalla el resultado.
            cout << "El Crupier se planta. Cartas: ";
            mem->crupier.plantado = true;
            for(int i = 0; i < mem->crupier.num_cartas; i++){
                if(i + 1 == mem->crupier.num_cartas){
                    cout << mem->crupier.cartas_jugador[i].figura << "-" << mem->crupier.cartas_jugador[i].pinta << ". ";
                } else {
                    cout << mem->crupier.cartas_jugador[i].figura << "-" << mem->crupier.cartas_jugador[i].pinta << " / " ;
                }
            }   
            cout << "Puntos: " << mem->crupier.puntos_ronda << "\n";
            cout << " ============================================= Turno del Crupier =============================================\n" << endl;
            break;

            } else {
            cout << "Tecla incorrecta. Vuelva a seleccionar una opción válida" << endl; //Caso que sirve por si el usuario oprime otra tecla que no sea la que corresponde
        }
    }

}

void determinarGanadorRonda(MemoriaCompartida *mem){
    int jugadores_ganan = 0; //Cuenta la cantidad de jugadores que le ganaron al Crupier
    int empate = 0; //Cuenta la cantidad de jugadores que empataron con Crupier

    for(int i = 0; i < NUM_JUGADORES; i++){
        if(mem->jugadores[i].perdio){
            cout << "Jugador " << i+1 << " se pasó de los puntos máximos.\n"; //Si el jugador se paso de 21 puntos, imprime el resultado por pantalla
            continue;
        }
        //El jugador no perdió
        if (mem->crupier.perdio || (!mem->jugadores[i].perdio && mem->jugadores[i].puntos_ronda > mem->crupier.puntos_ronda)) {
            //Si el Crupier perdió, o si los puntos del jugador son mayores a los del Crupier se le da la victoria al Jugador y se suman los puntos
            jugadores_ganan++;
            cout << "Jugador " << i+1 << " gana la ronda!\n";
            mem->jugadores[i].puntos += 1;
        } else if(mem->crupier.puntos_ronda == MAX_PUNTOS && mem->jugadores[i].puntos_ronda == MAX_PUNTOS){
            //Si crupier y jugador tienen 21 puntos, se le da por ganador a aquel que tenga menos cartas. Si gana el jugador se suman los puntos.
            if(mem->crupier.num_cartas <= mem->jugadores[i].num_cartas){
                cout << "Jugador " << i+1 << " perdió con el Crupier\n";
            } else {
                jugadores_ganan++;
                cout << "Jugador " << i+1 << " gana la ronda!\n";
                mem->jugadores[i].puntos += 1;
            }
        } else if(mem->crupier.puntos_ronda == mem->jugadores[i].puntos_ronda){
            //Si tienen los mismos puntos, se da como empate.
            cout << "Jugador " << i+1 << " no le ganó al Crupier\n";
            empate++;
        } else {
            //caso por si no entra a ninguno de los ifs anteriores y tener un respaldo.
            cout << "Jugador " << i+1 << " perdió con el Crupier\n";
        }
    }
    //Se reparten lo puntos al Crupier segun corresponda
    if(jugadores_ganan == 0 && empate == 0){
        //Si todos los jugadores pierden
        mem->crupier.puntos += 2;
        cout << "Crupier gana 2 puntos (todos los jugadores perdieron)" << endl;
    }else if(jugadores_ganan < (NUM_JUGADORES / 2) && empate < (NUM_JUGADORES / 2)){
        //Si le ganó a 3 jugadores (la mayoría)
        mem->crupier.puntos += 1;
        cout << "Crupier ganó 1 punto (ganó a la mayoría de los jugadores)" << endl;
    } else if(jugadores_ganan == NUM_JUGADORES/2) {
        //Si perdió con dos jugadores. (Implica perder con dos, ganar uno, empatar uno y todo lo que sea no ganarle a la mayoría [ganarle máximo a 2])
        cout << "Crupier no ganó ningun punto esta ronda (El Crupier el ganó a solo 2 o menos jugadores) " << endl;
    } else {
        //El crupier perdió con la mayoria de los jugadores
        cout << "Crupier no ganó ningun punto esta ronda (3 jugadores o más ganaron) " << endl;
    }

}

void mostrarEstado(MemoriaCompartida *mem){
    cout << "\n============================================= Estado actual =============================================" << endl;
    //Imprime los puntos de los jugadores
    for(int i = 0; i < NUM_JUGADORES; i++){
        cout << "Jugador " << i+1 << ": " << mem->jugadores[i].puntos << " puntos." << endl;
        cout << endl;
    }
    //Imprime los puntos del crupier
    cout << "Crupier: " << mem->crupier.puntos << " puntos.";
    cout << endl;

    cout << "============================================= Estado actual =============================================" << endl;
    cout << endl;
}

void mostrarGanadorFinal(MemoriaCompartida *mem) {
    int max_puntos = mem->crupier.puntos;
    vector<int> ganadores;
    // Buscar puntuación máxima
    for(int i = 0; i < NUM_JUGADORES; i++) {
        if(mem->jugadores[i].puntos > max_puntos) {
            max_puntos = mem->jugadores[i].puntos;
        }
    }
    // Identificar ganadores (Todo aquel que tenga los mismo puntos máximos o el que los tenga en solitario)
    if(mem->crupier.puntos == max_puntos) ganadores.push_back(-1);
    for(int i = 0; i < NUM_JUGADORES; i++) {
        if(mem->jugadores[i].puntos == max_puntos) ganadores.push_back(i);
    }
    // Construir mensaje que se va a imprimir
    stringstream mensaje;
    mensaje << "\nGANADOR(ES): ";
    for(int g : ganadores) {
        if(g == -1) mensaje << "Crupier ";
        else mensaje << "Jugador " << g+1 << " ";
    }
    if(ganadores.size() > 1) mensaje << "(Empate) "; //En caso de empate se imprimen los jugadores que empataron con la información de que se empató
    mensaje << "con " << max_puntos << " puntos!" << endl;
    
    cout << mensaje.str();
}