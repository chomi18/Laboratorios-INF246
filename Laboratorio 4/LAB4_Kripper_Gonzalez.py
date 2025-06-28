import threading
import random
import time
from pathlib import Path

#Configuración inicial
numeroCelulas = 512
celulasAlienigenas = 16
cantidadRondas = 5
probContagio = 0.7
duracionRonda = 10

#Sincronización
bloqueoEstado = threading.Lock()
barreraRonda = threading.Barrier(numeroCelulas + 1)
simulacionActiva = threading.Event()
simulacionActiva.set()

#Variables para el bonus
bonus_activado = input("¿Desea activar el bonus? (s/n): ").strip().lower() == 's'
if bonus_activado:
    evento_anticuerpo = threading.Event()
    terminado_anticuerpo = threading.Event()

#Clase Celula
class Celula(threading.Thread):
    def __init__(self, identificador, esAlienigena):
        super().__init__(name=f"Celula-{identificador:03d}")
        self.identificador = identificador
        self.esAlienigena = esAlienigena
        self.rondaInfeccion = 0 if esAlienigena else None
        self.historial = [f"R0: {'Alienígena' if esAlienigena else 'Humano'}"]
        self.oponente = None
        self.eliminada = False  #Atributo para el bonus
        self.curada = False     #Atributo para el bonus

    #Marca la celula como infectada (alienígena) 
    def marcarInfectada(self, ronda): 
        with bloqueoEstado:
            if not self.esAlienigena and not self.eliminada:
                self.esAlienigena = True
                self.rondaInfeccion = ronda
                self.historial.append(f"R{ronda}: Infectada")

    #Función para el bonus: Si la célula fue infectada hace máximo 2 rondas, la cura y vuelve a ser célula humana
    def curar(self, ronda):
        with bloqueoEstado:
            if self.esAlienigena and not self.eliminada:
                if self.rondaInfeccion and (ronda - self.rondaInfeccion) <= 2:
                    self.esAlienigena = False
                    self.rondaInfeccion = None
                    self.curada = True
                    self.historial.append(f"R{ronda}: Curada por anticuerpo")
                    return True
        return False

    #Realiza la lógica de combate según lo especificado en el enunciado
    def combatir(self, ronda):
        if self.eliminada:
            return

        if self.esAlienigena ^ self.oponente.esAlienigena:
            atacante = self if self.esAlienigena else self.oponente
            objetivo = self.oponente if atacante is self else self

            if random.random() < probContagio:
                objetivo.marcarInfectada(ronda)
                atacante.historial.append(f"R{ronda}: Contagió a {objetivo.name}")
            else:
                atacante.historial.append(f"R{ronda}: Falló contagio a {objetivo.name}")
                objetivo.historial.append(f"R{ronda}: Resistió contagio de {atacante.name}")
        else:
            self.historial.append(f"R{ronda}: Sin cambio")
            self.oponente.historial.append(f"R{ronda}: Sin cambio")

    #Corre la hebra correspondiente a cada célula
    def run(self):
        ronda = 1
        while simulacionActiva.is_set() and ronda <= cantidadRondas:
            barreraRonda.wait()
            
            #Saltar combate si está eliminada
            if not self.eliminada and self.oponente and self.identificador < self.oponente.identificador:
                self.combatir(ronda)
            
            barreraRonda.wait()
            ronda += 1

#Clase Anticuerpo para el bonus
class Anticuerpo(threading.Thread):
    def __init__(self, celulas):
        super().__init__(name="Anticuerpo")
        self.celulas = celulas

    #Corre la hebra correspondiente al anticuerpo
    def run(self):
        for ronda in range(2, cantidadRondas + 1):  #Se activa desde ronda 2
            evento_anticuerpo.wait()  #Esperar señal del main
            self.acciones_ronda(ronda)
            evento_anticuerpo.clear()
            terminado_anticuerpo.set()  # Indicar fin de acciones

    #Realiza las acciones del anticuerpo según lo especificado en el enunciado y las escribe en el archivo correspondiente
    def acciones_ronda(self, ronda):
        acciones = []
        celulas_disponibles = [c for c in self.celulas if not c.eliminada]
        random.shuffle(celulas_disponibles)
        
        for celula in celulas_disponibles[:4]:  #Máximo 4 interacciones por ronda
            with bloqueoEstado:
                if celula.eliminada:
                    continue
                    
                if celula.esAlienigena:
                    celula.eliminada = True
                    celula.historial.append(f"R{ronda}: Eliminada por anticuerpo")
                    acciones.append(f"Ronda {ronda} – Ataca {celula.name} (Alienígena) – Resultado: Eliminada")
                elif celula.rondaInfeccion and (ronda - celula.rondaInfeccion) <= 2:
                    if celula.curar(ronda):
                        acciones.append(f"Ronda {ronda} – Cura {celula.name} (Infectada) – Resultado: Curada")
                    else:
                        acciones.append(f"Ronda {ronda} – Ataca {celula.name} (Humana) – Resultado: No afecta")
                else:
                    acciones.append(f"Ronda {ronda} – Ataca {celula.name} (Humana) – Resultado: No afecta")
        
        #Registrar acciones en archivo
        with open("acciones_anticuerpo.txt", "a", encoding="utf-8") as f:
            for accion in acciones:
                f.write(accion + "\n")

#Funciones de escritura
def escribirAislamiento(celulas):
    with Path("aislamiento.txt").open("w", encoding="utf-8") as archivo:
        for c in celulas:
            tipo = "Alienígena" if c.esAlienigena else "Humano"
            archivo.write(f"{c.name} {tipo}\n")

def escribirRonda(celulas, ronda):
    with Path(f"ronda_{ronda}.txt").open("w", encoding="utf-8") as archivo:
        for c in celulas:
            if not c.eliminada:
                archivo.write(f"{c.name}: {c.historial[-1]}\n")

def escribirDiagnosticoFinal(celulas):
    with Path("diagnostico_final.txt").open("w", encoding="utf-8") as archivo:
        for c in celulas:
            if c.eliminada:
                archivo.write(f"{c.name} Eliminada\n")
            else:
                estado = "Alienígena" if c.esAlienigena else "Humana"
                if c.rondaInfeccion:
                    archivo.write(f"{c.name} {estado} (R{c.rondaInfeccion})\n")
                else:
                    archivo.write(f"{c.name} {estado}\n")

#Emparejamiento de enfrentamientos
def emparejar(celulas):
    celulas_activas = [c for c in celulas if not c.eliminada]
    indices = list(range(len(celulas_activas)))
    random.shuffle(indices)
    
    for i in range(0, len(indices), 2):
        if i + 1 >= len(indices):
            celulas_activas[indices[i]].oponente = None
            continue
        a = celulas_activas[indices[i]]
        b = celulas_activas[indices[i + 1]]
        a.oponente = b
        b.oponente = a

#Función principal
def main():
    global bonus_activado

    #Inicializar archivo de anticuerpo
    if bonus_activado and Path("acciones_anticuerpo.txt").exists():
        Path("acciones_anticuerpo.txt").unlink()

    randomsAliens = set(random.sample(range(numeroCelulas), celulasAlienigenas))
    celulas = [Celula(i, i in randomsAliens) for i in range(numeroCelulas)]
    
    escribirAislamiento(celulas)

    #Iniciar hebra de anticuerpo si es necesario
    if bonus_activado:
        anticuerpo = Anticuerpo(celulas)
        anticuerpo.start()

    for c in celulas:
        c.start()

    for ronda in range(1, cantidadRondas + 1):
        emparejar(celulas)
        inicio = time.time()

        barreraRonda.wait()
        barreraRonda.wait()

        #Ejecutar acciones del anticuerpo
        if bonus_activado and ronda >= 2:
            terminado_anticuerpo.clear()
            evento_anticuerpo.set()
            terminado_anticuerpo.wait()

        #Control de tiempo por ronda
        restante = duracionRonda - (time.time() - inicio)
        if restante > 0:
            time.sleep(restante)

        escribirRonda(celulas, ronda)

    simulacionActiva.clear()
    for c in celulas:
        c.join()
    
    if bonus_activado:
        evento_anticuerpo.set()  #Asegurar fin del anticuerpo
        anticuerpo.join()

    escribirDiagnosticoFinal(celulas)

if __name__ == "__main__":
    main()