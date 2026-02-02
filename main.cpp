/******************************************************************************
 * Suma de arreglos en paralelo con OpenMP
 * Descripción:
 *  - Se generan dos arreglos A y B de tamaño N=1000 con valores aleatorios [1,1000].
 *  - Se calcula el arreglo C = A + B de forma:
 *      (1) Secuencial
 *      (2) Paralela usando OpenMP con #pragma omp parallel for
 *  - Se usa schedule(static, chunk=100) para controlar la asignación de trabajo.
 *  - Se muestran únicamente los primeros 10 elementos (mostrar=10) para verificar resultados.
 *  - Se miden tiempos (ms) de ambas ejecuciones para comparar desempeño.
 *
 ******************************************************************************/

#include <iostream>     // Entrada/salida estándar
#include <random>       // Números aleatorios (random_device, mt19937, distribution)
#include <chrono>       // Medición de tiempos
#include <iomanip>      // Formato de salida (setprecision)
#include <omp.h>        // OpenMP

// ===============================
// Constantes del programa
// ===============================
#define N 1000       // Tamaño de los arreglos
#define chunk 100    // Tamaño del bloque (chunk) para schedule
#define mostrar 10   // Cuántos elementos imprimir para verificación

using namespace std;

// ===============================
// Imprime solo los primeros "mostrar" elementos del arreglo
// ===============================
void imprimeArreglo(const int *d, const char *nombre);

int main() {

    cout << "Sumando Arreglos en Paralelo (OpenMP)\n";

    // ===============================
    // 1) Declaración de arreglos 
    // ===============================
    int A[N], B[N], C_seq[N], C_par[N];

    // ===============================
    // 2) Inicialización: números aleatorios en [1, 1000]
    //    - random_device: fuente de entropía
    //    - mt19937: generador
    //    - uniform_int_distribution: distribución uniforme
    // ===============================
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 1000);

    for (int i = 0; i < N; i++) {
        A[i] = dist(gen);
        B[i] = dist(gen);
        // Inicializamos resultados por claridad
        C_seq[i] = 0;
        C_par[i] = 0;
    }

    // ===============================
    // 3) SUMA SECUENCIAL (baseline)
    //    - Cada iteración depende solo de A[i] y B[i]
    //    - No hay paralelismo, corre en un solo hilo
    // ===============================
    auto inicio_seq = chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        C_seq[i] = A[i] + B[i];
    }

    auto fin_seq = chrono::high_resolution_clock::now();
    double tiempo_secuencial =
        chrono::duration<double, milli>(fin_seq - inicio_seq).count();

    // ===============================
    // 4) SUMA PARALELA CON OPENMP
    //    - Se paraleliza el for.
    //    - shared(): arreglos compartidos entre hilos.
    //    - private(i): i privada por hilo (evita condiciones de carrera).
    //    - schedule(static, chunk): reparte el trabajo en bloques fijos de 100 iteraciones.
    // ===============================
    int pedazos = chunk;

    auto inicio_par = chrono::high_resolution_clock::now();

#pragma omp parallel for shared(A, B, C_par, pedazos) schedule(static, pedazos)
    for (int i = 0; i < N; i++) {
        // Cada hilo escribe en una posición distinta C_par[i], por lo que es seguro
        C_par[i] = A[i] + B[i];
    }

    auto fin_par = chrono::high_resolution_clock::now();
    double tiempo_paralelo =
        chrono::duration<double, milli>(fin_par - inicio_par).count();

    // ===============================
    // 5) Verificación rápida:
    //    - Imprimimos solo 10 elementos para validar visualmente
    //    - Validamos que C_seq y C_par coincidan en todos los índices
    // ===============================
    imprimeArreglo(A, "A");
    imprimeArreglo(B, "B");
    imprimeArreglo(C_par, "C (paralelo)");

    bool correcto = true;
    for (int i = 0; i < N; i++) {
        if (C_seq[i] != C_par[i]) {
            correcto = false;
            // Reporte mínimo del primer error encontrado
            cout << "\n[ERROR] Diferencia en i=" << i
                 << " C_seq=" << C_seq[i]
                 << " C_par=" << C_par[i] << endl;
            break;
        }
    }

    cout << "\nVerificacion (C_seq == C_par): " << (correcto ? "OK" : "FALLO") << endl;

    // ===============================
    // 6) Reporte de tiempos (ms)
    //    - Nota: En tamaños pequeños o entornos virtualizados, el paralelo puede salir más lento
    //    - Para ver ganancia real, suele requerirse más N o más trabajo por iteración
    // ===============================
    cout << fixed << setprecision(4);
    cout << "\nTiempo secuencial (ms): " << tiempo_secuencial << endl;
    cout << "Tiempo paralelo   (ms): " << tiempo_paralelo << endl;

    return 0;
}

// ===============================
// Función de impresión parcial
// ===============================
void imprimeArreglo(const int *d, const char *nombre) {
    cout << "Arreglo " << nombre << " (primeros " << mostrar << "): ";
    for (int x = 0; x < mostrar; x++) {
        cout << d[x] << " ";
    }
    cout << endl;
}