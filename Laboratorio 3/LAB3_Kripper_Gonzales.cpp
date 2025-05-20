
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <filesystem>
#include <sys/wait.h>
#include <unistd.h>
#include <array>
#include <chrono>
namespace fs = std::filesystem;

using namespace std;

using Matriz = vector<vector<int>>;
using Entero = int;

Matriz leerMatriz(ifstream &archivo, Entero filas, Entero columnas){
    Matriz matriz(filas, vector<int>(columnas));
    for (Entero f = 0; f < filas; ++f)
        for (Entero c = 0; c < columnas; ++c)
            archivo >> matriz[f][c];

    return matriz;
}

Matriz multiplicarFork(const Matriz& A, const Matriz& B) {

    Entero filas  = A.size();      // filas de A
    Entero colsB  = B[0].size();   // columnas de B
    Entero comun  = A[0].size();

    Matriz C(filas, vector<int>(colsB, 0));

    vector<array<int,2>> pipes(filas);   // un pipe por fila

    for (Entero f = 0; f < filas; ++f) {

        if (pipe(pipes[f].data()) == -1)
            throw runtime_error("no se pudo crear la pipe");

        pid_t pid = fork();
        if (pid < 0) throw runtime_error("fork falló");

        if (pid == 0) {
            close(pipes[f][0]);

            ostringstream oss;
            for (Entero c = 0; c < colsB; ++c) {
                Entero acc = 0;
                for (Entero k = 0; k < comun; ++k)
                    acc += A[f][k] * B[k][c];
                oss << acc << (c + 1 == colsB ? '\n' : ' ');
            }
            string filaTxt = oss.str();
            write(pipes[f][1], filaTxt.c_str(), filaTxt.size());
            _exit(0);
        }

        close(pipes[f][1]);
    }

    // leo cada fila que llega como texto
    for (Entero f = 0; f < filas; ++f) {
        FILE* fp = fdopen(pipes[f][0], "r");
        for (Entero c = 0; c < colsB; ++c)
            fscanf(fp, "%d", &C[f][c]);
        fclose(fp);
    }

    while (wait(nullptr) > 0);

    return C;

}

void procesarArchivos(const fs::path& ruta, ofstream& salida){

    ifstream entrada(ruta);

    Entero cantidadMatrices;
    entrada >> cantidadMatrices;

    Entero filasA, columnasA;
    entrada >> filasA >> columnasA;
    Matriz A = leerMatriz(entrada, filasA, columnasA);

    entrada.ignore(numeric_limits<streamsize>::max(), '\n');

    Entero filasB, columnasB;
    entrada >> filasB >> columnasB;
    Matriz B = leerMatriz(entrada, filasB, columnasB);

    if (columnasA != filasB) {
        return;
    }

    auto tiempo_inicial = chrono::high_resolution_clock::now();

    Matriz C = multiplicarFork(A, B);

    auto tiempo_final = chrono::high_resolution_clock::now();

    chrono::duration<double> duracion = tiempo_final - tiempo_inicial;

    salida << ruta.filename().string() << "\n\n";

    for (size_t i = 0; i < C.size(); ++i) {
        for (size_t j = 0; j < C[i].size(); ++j)
            salida << C[i][j] << (j + 1 == C[i].size() ? '\n' : ' ');
    }
    salida << '\n';

    salida << "tiempo = " << duracion.count() << " segundos\n\n";

}

int main(){


    // acá cambiar el path del directorio según convenga
    const fs::path carpetaData = "/Users/davidkripper/Library/CloudStorage/OneDrive-Personal/Universidad/FourthY/1/SO/Laboratorios-INF246/Laboratorio 3/archivos_de_prueba/data";

    ofstream salida("salidaFork.txt", ios::trunc);
    
    for (const auto& entrada : fs::directory_iterator(carpetaData)){
        if (entrada.is_regular_file() && entrada.path().extension() == ".txt"){
            procesarArchivos(entrada.path(), salida);
        }
    }

    return 0;
}