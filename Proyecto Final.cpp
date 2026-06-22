#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <sstream>
using namespace std;
struct Jugador {
    string nombre;
    int edad;
    int saldo;
    vector<string> historial;
};
bool cargarPerfil(Jugador& p);
void guardarSesion(const Jugador& p);
void menuPrincipal(Jugador& p);
bool realizarApuesta(Jugador& p, int& apuesta);
void jugarBlackjack(Jugador& p, int apuesta);
void jugarTragamonedas(int& saldo, int apuesta);
void jugarAdivinaNumero(int& saldo, int apuesta);
bool cargarPerfil(Jugador& p) {
    ifstream archivo("input/perfil.txt");
    if (!archivo.is_open()) return false;

    string linea, temp;
    if (getline(archivo, linea)) {
        stringstream ss(linea);

        getline(ss, p.nombre, ',');
        
        getline(ss, temp, ',');
        p.edad = stoi(temp); 
        
        getline(ss, temp, ',');
        p.saldo = stoi(temp);
        
        archivo.close();
        return true;
    }
    archivo.close();
    return false;
}
void guardarSesion(const Jugador& p) {
    ofstream archivo("input/perfil.txt"); 
    
    if (archivo.is_open()) {
        archivo << p.nombre << "," << p.edad << "," << p.saldo;
        
        archivo.close();
        cout << "\n[Sistema] Sesion guardada correctamente con saldo: $" << p.saldo << endl;
    } else {
        cerr << "[Error] No se pudo abrir el archivo para guardar el saldo." << endl;
    }
}

bool realizarApuesta(Jugador& p, int& apuesta) {
    cout << "\nTu saldo actual: $" << p.saldo << endl;
    cout << "Ingresa tu apuesta (0 para volver al menú): ";
    cin >> apuesta;
    
    if (apuesta == 0) return false;
    if (apuesta > p.saldo) {
        cout << "¡Saldo insuficiente! Intenta con un monto menor." << endl;
        return realizarApuesta(p, apuesta); 
    }
    return true;
}
void registrarResultado(Jugador& p, string nombreJuego, int monto) {
    string registro = nombreJuego + (monto >= 0 ? " (Ganado: +" : " (Perdido: ") 
                      + to_string(monto) + ")";
    p.historial.push_back(registro);
}

void menuPrincipal(Jugador& p) {
    int opcion, apuesta;
    bool continuar = true;

    while (continuar) {
        cout << "\n--- CASINO VIRTUAL INTERACTIVO ---" << endl;
        cout << "Saldo actual: $" << p.saldo << endl;
        cout << "1. Blackjack" << endl;
        cout << "2. Tragamonedas" << endl;
        cout << "3. Adivina el Numero" << endl;
        cout << "4. Salir y Guardar" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        if (opcion == 4) {
            guardarSesion(p);
            cout << "Sesion guardada. ¡Hasta pronto!" << endl;
            continuar = false;
        } 
        else if (opcion >= 1 && opcion <= 3) {
            if (realizarApuesta(p, apuesta)) {
                int saldoAntes = p.saldo;
                
                switch (opcion) {
                    case 1: 
                        jugarBlackjack(p, apuesta); 
                        break;
                    case 2: 
                        jugarTragamonedas(p.saldo, apuesta); 
                        break;
                    case 3: 
                        jugarAdivinaNumero(p.saldo, apuesta); 
                        break;
                }
                
                int diferencia = p.saldo - saldoAntes;
                string nombresJuegos[] = {"", "Blackjack", "Tragamonedas", "Adivina el Numero"};
                registrarResultado(p, nombresJuegos[opcion], diferencia);
                
                cout << "Resultado de la jugada: " << (diferencia >= 0 ? "Ganaste " : "Perdiste ") 
                     << abs(diferencia) << "$" << endl;
            }
        } 
        else {
            cout << "Opcion no valida, intente de nuevo." << endl;
        }
    }
}

