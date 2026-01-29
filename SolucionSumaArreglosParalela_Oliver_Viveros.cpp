/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
// ===============================
// Inclusión de librerías estándar
// ===============================
#include <iostream>     // Entrada y salida estándar
#include <vector>       // Arreglos dinámicos
#include <random>       // Generación de números aleatorios
#include <chrono>       // Medición de tiempo
#include <iomanip>      // Formato de salida
#include <omp.h>        // Librería OpenMP

using namespace std;

int main() {

    // ===============================
    // Tamaño fijo del arreglo
    // ===============================
    const int n = 10;   // Por defecto 10 elementos

    // ===============================
    // Declaración de arreglos
    // ===============================
    vector<int> A(n), B(n), R(n), Rpar(n);

    // ===============================
    // Generación de números aleatorios
    // ===============================
    random_device rd;                    // Dispositivo de entropía
    mt19937 gen(rd());                   // Generador Mersenne Twister
    uniform_int_distribution<int> dist(1, 1000); // Rango [1, 1000]

    for (int i = 0; i < n; i++) {
        A[i] = dist(gen);
        B[i] = dist(gen);
    }

    // ===============================
    // Definir número de hilos
    // ===============================
    omp_set_num_threads(2); // Puedes cambiar a 2, 4, etc.

    // ===============================
    // SUMA SECUENCIAL
    // ===============================
    auto inicio_seq = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        R[i] = A[i] + B[i];
    }

    auto fin_seq = chrono::high_resolution_clock::now();
    double tiempo_secuencial =
        chrono::duration<double, milli>(fin_seq - inicio_seq).count();

    // ===============================
    // SUMA PARALELA CON OPENMP
    // ===============================
    auto inicio_par = chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (int i = 0; i < n; i++) {
        Rpar[i] = A[i] + B[i];
    }

    auto fin_par = chrono::high_resolution_clock::now();
    double tiempo_paralelo =
        chrono::duration<double, milli>(fin_par - inicio_par).count();

    // ===============================
    // Impresión de resultados
    // ===============================
    cout << "Arreglo A: ";
    for (int i = 0; i < n; i++) cout << A[i] << " ";
    cout << endl;

    cout << "Arreglo B: ";
    for (int i = 0; i < n; i++) cout << B[i] << " ";
    cout << endl;

    cout << "Arreglo Resultado (Paralelo): ";
    for (int i = 0; i < n; i++) cout << Rpar[i] << " ";
    cout << endl;

    // ===============================
    // Mostrar tiempos
    // ===============================
    cout << fixed << setprecision(4);
    cout << "\nTiempo secuencial (ms): " << tiempo_secuencial << endl;
    cout << "Tiempo paralelo   (ms): " << tiempo_paralelo << endl;

    return 0;
}