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
struct Card {
    string name;
    int value;
};

int getScore(const vector<Card>& hand) {
    int score = 0, aces = 0;
    for (const auto& c : hand) {
        score += c.value;
        if (c.value == 11) aces++;
    }
    while (score > 21 && aces > 0) { score -= 10; aces--; }
    return score;
}

void printHand(string owner, const vector<Card>& hand, bool hideFirst = false) {
    cout << owner << ": ";
    if (hideFirst) cout << "[Oculta] " << hand[1].name;
    else {
        for (const auto& c : hand) cout << c.name << " ";
    }
    cout << " | Puntuacion: " << (hideFirst ? (hand[1].value == 11 ? 11 : hand[1].value) : getScore(hand)) << endl;
}

void jugarBlackjack(Jugador& p, int apuesta) {
    vector<string> names = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    vector<int> values = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11};
    vector<Card> deck;

    for (int i = 0; i < 6; ++i) {
        for (size_t j = 0; j < names.size(); ++j) deck.push_back({names[j], values[j]});
    }

    shuffle(deck.begin(), deck.end(), mt19937(static_cast<unsigned>(time(0))));

    vector<Card> pHand, dHand;
    for (int i = 0; i < 2; ++i) {
        pHand.push_back(deck.back()); deck.pop_back();
        dHand.push_back(deck.back()); deck.pop_back();
    }

    char opt;
    while (getScore(pHand) < 21) {
        printHand("Crupier", dHand, true);
        printHand("Jugador", pHand);
        cout << "Pedir carta? (s/n): ";
        cin >> opt;
        if (opt == 's') {
            pHand.push_back(deck.back()); deck.pop_back();
        } else break;
    }

    if (getScore(pHand) <= 21) {
        while (getScore(dHand) < 17) {
            dHand.push_back(deck.back()); deck.pop_back();
        }
    }

    cout << "\n--- Resultado Final ---" << endl;
    printHand("Crupier", dHand);
    printHand("Jugador", pHand);

    int sP = getScore(pHand), sD = getScore(dHand);
    if (sP > 21) {
        cout << "Te has pasado. Pierdes." << endl;
        p.saldo -= apuesta;
    } else if (sD > 21 || sP > sD) {
        cout << "¡Ganaste!" << endl;
        p.saldo += apuesta;
    } else if (sP == sD) {
        cout << "Empate." << endl;
    } else {
        cout << "El crupier gana." << endl;
        p.saldo -= apuesta;
    }
}

void jugarTragamonedas(int& saldo, int apuesta) {
    int r1 = rand() % 10, r2 = rand() % 10, r3 = rand() % 10;
    cout << "\nGirando... | " << r1 << " | " << r2 << " | " << r3 << " |" << endl;
   
    if (r1 == r2 && r2 == r3) {
        cout << "¡JACKPOT!" << endl;
        saldo += (apuesta * 9);
    } else if (r1 == r2 || r2 == r3 || r1 == r3) {
        cout << "¡Dos iguales!" << endl;
        saldo += apuesta;
    } else {
        cout << "Perdiste." << endl;
        saldo -= apuesta;
    }
}

bool intentarAdivinar(int numeroSecreto, int intentos) {
    if (intentos == 0) return false;
    int intento;
    cout << "Intentos: " << intentos << ". Numero: "; cin >> intento;
    if (intento == numeroSecreto) return true;
    cout << (intento < numeroSecreto ? "Mas alto..." : "Mas bajo...") << endl;
    return intentarAdivinar(numeroSecreto, intentos - 1);
}

void jugarAdivinaNumero(int& saldo, int apuesta) {
    int rangoMaximo = 20;
    int numeroSecreto = rand() % rangoMaximo + 1;
    int intentosTotales = 4;
    cout << "\n--- ADIVINA EL NUMERO ---" << endl;
    cout << "Tienes que adivinar un numero entre 1 y " << rangoMaximo << "." << endl;
    cout << "Tienes " << intentosTotales << " intentos." << endl;

    if (intentarAdivinar(numeroSecreto, intentosTotales)) {
        cout << "¡Felicidades! Has ganado." << endl;
        saldo += apuesta;
    } else {
        cout << "Has perdido. El numero secreto era: " << numeroSecreto << endl;
        saldo -= apuesta;
    }
}
int main() {
    srand(static_cast<unsigned int>(time(0)));
    Jugador p;

    cout << "=== BIENVENIDO AL CASINO VIRTUAL ===" << endl;

    if (!cargarPerfil(p)) {
        cout << "El sistema no puede continuar sin un perfil valido." << endl;
        return 0;
    }

    cout << "Bienvenido, " << p.nombre << ". Saldo inicial: $" << p.saldo << endl;
    menuPrincipal(p);

    return 0;
}
