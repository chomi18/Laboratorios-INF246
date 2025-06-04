
// ARCHIVO PARA BONUS

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

vector<Matriz> leerTodasLasMatrices(ifstream &in){
    int n;  in >> n;
    vector<Matriz> mats;

    for (int k = 0; k < n; ++k){
        int r, c;  in >> r >> c;
        mats.push_back(leerMatriz(in, r, c));
        in.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return mats;
}

bool MatricesCompatibles(const vector<Matriz>& Matriz){
    for (size_t i = 0; i + 1 < Matriz.size(); ++i)
        if (Matriz[i][0].size() != Matriz[i+1].size())
            return false;
    return true;
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

Matriz multiplicarCadena(const vector<Matriz>& M){

    Matriz acc = M.front();
    for (size_t i = 1; i < M.size(); ++i)
        acc = multiplicarFork(acc, M[i]);
    return acc;
}

bool MatrizEsSimetrica(const Matriz& A){

    if (A.size() != A[0].size()) return false;
    for (size_t i = 0; i < A.size(); ++i)
        for (size_t j = i + 1; j < A.size(); ++j)
            if (A[i][j] != A[j][i]) return false;
    return true;

}

void procesarArchivos(const fs::path& ruta, ofstream& salida){

    ifstream entrada(ruta);

    vector<Matriz> mats = leerTodasLasMatrices(entrada);
    if (mats.empty()) return;

    if (!MatricesCompatibles(mats))
        return;

    auto tiempo_inicial = chrono::high_resolution_clock::now();
    Matriz C = multiplicarCadena(mats);
    auto tiempo_final = chrono::high_resolution_clock::now();
    chrono::duration<double> duracion = tiempo_final - tiempo_inicial;

    salida << ruta.filename().string() << "\n\n";
    for (size_t i = 0; i < C.size(); ++i){
        for (size_t j = 0; j < C[i].size(); ++j)
            salida << C[i][j] << (j + 1 == C[i].size() ? '\n' : ' ');
    }
    salida << '\n';
    salida << "tiempo = " << duracion.count() << " segundos\n";
    salida << "simetrica = " << (MatrizEsSimetrica(C) ? "SI" : "NO") << "\n\n";
}

int main(){

    // acá cambiar el path del directorio según convenga
    const fs::path carpetaData = "/Users/davidkripper/Library/CloudStorage/OneDrive-Personal/Universidad/FourthY/1/SO/Laboratorios-INF246/Laboratorio 3/archivos_de_prueba/bonus/medium";

    ofstream salida("salidaForkBonus.txt", ios::trunc);
    
    for (const auto& entrada : fs::directory_iterator(carpetaData)){
        if (entrada.is_regular_file() && entrada.path().extension() == ".txt"){
            procesarArchivos(entrada.path(), salida);
        }
    }

    return 0;
}