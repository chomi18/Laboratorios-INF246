
import threading
import random
import time
from pathlib import Path

numeroCelulas = 512
celulasAlienigenas = 16
cantidadRondas = 5
probContagio = 0.7
duracionRonda = 10

bloqueoEstado = threading.Lock()
barreraRonda = threading.Barrier(numeroCelulas + 1)

simulacionActiva = threading.Event()
simulacionActiva.set()

class Celula(threading.Thread):
    def __init__(self, identificador, esAlienigena):
        super().__init__(name = f"Celula-{identificador:03d}")
        self.identificador = identificador
        self.esAlienigena = esAlienigena
        self.rondaInfeccion = 0 if esAlienigena else None
        self.historial = [f"R0: {'Alienígena' if esAlienigena else 'Humano'}"]
        self.oponente = None

    def marcarInfectada(self, ronda):
        with bloqueoEstado:
            if not self.esAlienigena:
                self.esAlienigena = True
                self.rondaInfeccion = ronda
                self.historial.append(f"R{ronda}: Infectada")

    def combatir(self, ronda):
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

    def run(self):
        ronda = 1
        while simulacionActiva.is_set() and ronda <= cantidadRondas:
            barreraRonda.wait()

            if self.oponente and self.identificador < self.oponente.identificador:
                self.combatir(ronda)

            barreraRonda.wait()
            ronda += 1

def escribirAislamiento(celulas):
    with Path("aislamiento.txt").open("w", encoding="utf-8") as archivo:
        for c in celulas:
            tipo = "Alienígena" if c.esAlienigena else "Humano"
            archivo.write(f"{c.name} {tipo}\n")

def escribirRonda(celulas, ronda):
    with Path(f"ronda_{ronda}.txt").open("w", encoding="utf-8") as archivo:
        for c in celulas:
            archivo.write(f"{c.name}: {c.historial[-1]}\n")

def escribirDiagnosticoFinal(celulas):
    with Path("diagnostico_final.txt").open("w", encoding="utf-8") as archivo:
        for c in celulas:
            estado = "Alienígena" if c.esAlienigena else "Humano"
            if c.rondaInfeccion:
                archivo.write(f"{c.name} {estado} (R{c.rondaInfeccion})\n")
            else:
                archivo.write(f"{c.name} {estado}\n")

def emparejar(celulas):
    indices = list(range(len(celulas)))
    random.shuffle(indices)
    for i in range(0, len(indices), 2):
        a = celulas[indices[i]]
        b = celulas[indices[i + 1]]
        a.oponente = b
        b.oponente = a

def main():

    randomsAliens = set(random.sample(range(numeroCelulas), celulasAlienigenas))
    celulas = [Celula(i, i in randomsAliens) for i in range(numeroCelulas)]

    escribirAislamiento(celulas)

    for c in celulas:
        c.start()

    for ronda in range(1, cantidadRondas + 1):
        emparejar(celulas)

        inicio = time.time()

        barreraRonda.wait()
        barreraRonda.wait()

        restante = duracionRonda - (time.time() - inicio)
        if restante > 0:
            time.sleep(restante)

        escribirRonda(celulas, ronda)

    simulacionActiva.clear()
    for c in celulas:
        c.join()

    escribirDiagnosticoFinal(celulas)

if __name__ == "__main__":
    main()
