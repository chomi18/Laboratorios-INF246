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
    bool repartida;
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

void iniciarMazo(MemoriaCompartida *mem){
    char pintas[] = {'C', 'D', 'T', 'P'};
    const char* figuras[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int valores[] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    int indice = 0;

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
    mem->cartas_repartidas = 0;
    srand(time(NULL));
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
    mem->mazo[mem->cartas_repartidas].repartida = true;
    mem->cartas_repartidas++;
    return carta;
}

void JugadorJuega(int id, MemoriaCompartida *mem){
    usleep(100000*id);
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

    cout << "Jugador " << id+1 << " tiene las cartas: " << mem->jugadores[id].cartas_jugador[0].figura << "-" << mem->jugadores[id].cartas_jugador[0].pinta << 
    " y " << mem->jugadores[id].cartas_jugador[1].figura << "-" << mem->jugadores[id].cartas_jugador[1].pinta << " y tiene " << mem->jugadores[id].puntos_ronda << " puntos.\n";
    
    while(!mem->crupier_recibe_cartas){

    }

    usleep(100000*id);

    int revisador_A = 0; //Guarda la posición de donde se encontraba la última A
    while(mem->jugadores[id].puntos_ronda < MAX_PUNTOS){
        if(pedirCarta(mem->jugadores[id].puntos_ronda)){
            //Pide carta segun las reglas y la función pedirCarta()
            mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas] = repartirCarta(mem);
            mem->jugadores[id].puntos_ronda += mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].valor;
            //Informa por pantalla la información de la carta obtenida
            cout << "El Jugador " << id+1 << " obtuvo la carta: " << mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].figura << 
            "-" << mem->jugadores[id].cartas_jugador[mem->jugadores[id].num_cartas].pinta;
            mem->jugadores[id].num_cartas++;
            //Revisa si el Jugador tiene un As en su mano, si es así le resta 10 a los puntos de la mano
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
    string estado;
    if(mem->jugadores[id].puntos_ronda > MAX_PUNTOS){
        estado = "perdió";
        mem->jugadores[id].perdio = true;
    } else {
        estado = "se planta";
        mem->jugadores[id].plantado = true;
    }
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
    mem->crupier.perdio = false;
    mem->crupier.plantado = false;
    sleep(1);
    cout << "\n ============================================= Turno del Crupier ============================================= \n" << endl;

    mem->crupier.num_cartas = 0;
    mem->crupier.puntos_ronda = 0;
    //Carta 1
    mem->crupier.cartas_jugador[mem->crupier.num_cartas] = repartirCarta(mem);
    mem->crupier.puntos_ronda += mem->crupier.cartas_jugador[mem->crupier.num_cartas].valor;
    mem->crupier.num_cartas++;
    //Carta 2
    mem->crupier.cartas_jugador[mem->crupier.num_cartas] = repartirCarta(mem);
    mem->crupier.puntos_ronda += mem->crupier.cartas_jugador[mem->crupier.num_cartas].valor;
    mem->crupier.num_cartas++;

    cout << "Crupier tiene las cartas: " << mem->crupier.cartas_jugador[0].figura << "-" << mem->crupier.cartas_jugador[0].pinta << 
    " y [Carta boca abajo]" << endl;
    cout << endl;

    cout << " ============================================= Turno del Crupier =============================================\n" << endl;

    mem->crupier_recibe_cartas = true;

    for (int i = 0; i < NUM_JUGADORES; i++){
        waitpid(mem->jugadores[i].pid, NULL, 0); 
    }

    cout << " ============================================= Turno del Crupier =============================================" << endl;

    cout << "Crupier da vuelta su carta. Sus cartas son: " << mem->crupier.cartas_jugador[0].figura << "-" << mem->crupier.cartas_jugador[0].pinta << 
    " y " << mem->crupier.cartas_jugador[1].figura << "-" << mem->crupier.cartas_jugador[1].pinta << " y tiene " << mem->crupier.puntos_ronda << " puntos.\n" << endl;
    if(mem->crupier.puntos_ronda == MAX_PUNTOS) cout << " ============================ Turno del Crupier: ============================\n" << endl;

    int revisador_A = 0;
    while(mem->crupier.puntos_ronda < MAX_PUNTOS){
        char decision;
        cout << "Ingrese su desición. '(C) para pedir Carta o (P) para Plantarse: '";
        cin >> decision;
        cout << endl;
        if(decision == 'C'){
            mem->crupier.cartas_jugador[mem->crupier.num_cartas] = repartirCarta(mem);
            mem->crupier.puntos_ronda += mem->crupier.cartas_jugador[mem->crupier.num_cartas].valor;
            //Informa por pantalla la información de la carta obtenida
            cout << "Carta obrenida: " << mem->crupier.cartas_jugador[mem->crupier.num_cartas].figura << 
            "-" << mem->crupier.cartas_jugador[mem->crupier.num_cartas].pinta;
            mem->crupier.num_cartas++;

            //Verifica si el Crupier se pasó de puntos
            //Si la carta obtenida es un As, ajusta los puntos
            for(int i = revisador_A; i < mem->crupier.num_cartas; i++){
                if(strcmp(mem->crupier.cartas_jugador[i].figura, "A") == 0 && mem->crupier.puntos_ronda > MAX_PUNTOS){
                    mem->crupier.puntos_ronda -= 10;
                    revisador_A = i+1;
                    break;
                }
            }
            if(mem->crupier.puntos_ronda > MAX_PUNTOS){ //Si se pasa de los puntos, termina el ciclo de decision
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
        } else if(decision == 'P') {
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
            cout << "Tecla incorrecta. Vuelva a seleccionar una opción válida" << endl;
        }
    }

}

void determinarGanadorRonda(MemoriaCompartida *mem){
    int jugadores_ganan = 0;
    int empate = 0;

    for(int i = 0; i < NUM_JUGADORES; i++){
        if(mem->jugadores[i].perdio){
            cout << "Jugador " << i+1 << " se pasó de los puntos máximos.\n";
            continue;
        }

        if (mem->crupier.perdio || (!mem->jugadores[i].perdio && mem->jugadores[i].puntos_ronda > mem->crupier.puntos_ronda)) {
            jugadores_ganan++;
            cout << "Jugador " << i+1 << " gana la ronda!\n";
            mem->jugadores[i].puntos += 1;
        } else if(mem->crupier.puntos_ronda == MAX_PUNTOS && mem->jugadores[i].puntos_ronda == MAX_PUNTOS){
            if(mem->crupier.num_cartas <= mem->jugadores[i].num_cartas){
                cout << "Jugador " << i+1 << " perdió con el Crupier\n";
            } else {
                jugadores_ganan++;
                cout << "Jugador " << i+1 << " gana la ronda!\n";
            }
        } else if(mem->crupier.puntos_ronda == mem->jugadores[i].puntos_ronda){
            cout << "Jugador " << i+1 << " no le ganó al Crupier\n";
            empate++;
        } else {
            cout << "Jugador " << i+1 << " perdió con el Crupier\n";
        }
    }

    if(jugadores_ganan == 0 && empate == 0){
        mem->crupier.puntos += 2;
        cout << "Crupier gana 2 puntos (todos los jugadores perdieron)" << endl;
    }else if(jugadores_ganan < (NUM_JUGADORES / 2) && empate < (NUM_JUGADORES / 2)){
        mem->crupier.puntos += 1;
        cout << "Crupier ganó 1 punto (ganó a la mayoría de los jugadores)" << endl;
    } else if(jugadores_ganan == NUM_JUGADORES/2) {
        cout << "Crupier no ganó ningun punto esta ronda (El Crupier el ganó a solo 2 o menos jugadores) " << endl;
    } else {
        cout << "Crupier no ganó ningun punto esta ronda (3 jugadores o más ganaron) " << endl;
    }

}

void mostrarEstado(MemoriaCompartida *mem){
    cout << "\n============================================= Estado actual =============================================" << endl;
    for(int i = 0; i < NUM_JUGADORES; i++){
        cout << "Jugador " << i+1 << ": " << mem->jugadores[i].puntos << " puntos." << endl;
        cout << endl;
    }

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
    
    // Identificar ganadores
    if(mem->crupier.puntos == max_puntos) ganadores.push_back(-1);
    for(int i = 0; i < NUM_JUGADORES; i++) {
        if(mem->jugadores[i].puntos == max_puntos) ganadores.push_back(i);
    }
    
    // Construir mensaje
    stringstream mensaje;
    mensaje << "\nGANADOR(ES): ";
    
    for(int g : ganadores) {
        if(g == -1) mensaje << "Crupier ";
        else mensaje << "Jugador " << g+1 << " ";
    }
    
    if(ganadores.size() > 1) mensaje << "(Empate) ";
    mensaje << "con " << max_puntos << " puntos!\n";
    
    cout << mensaje.str();
}

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