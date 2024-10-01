#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <map>  // Asegúrate de incluir esta línea
#include <set>

using namespace std;



class Carta {
public:
    string tipo;
    string color;

    Carta(string tipo, string color) : tipo(tipo), color(color) {} // creacion del tipo de carta

    void mostrarCarta() const { // funcion para mostrar las cartas
        cout << tipo;
        if (!color.empty()) { // si color es diferente de vacio muestro el color
            cout << " " << color;
        }
        cout << endl;
    }
};

class Fila {
private:
    vector<Carta> cartas;
    const int maxCartas = 3;
    bool activa; // dice el estado de la fila 

public:
    Fila() : activa(true) {} // Constructor que inicializa la fila como activa

    // Método para verificar si la fila está activa
    bool esActiva() const { return activa; }

    bool agregarCarta(const Carta& carta) { // para que me pase una carta y no la duplique
        if (activa && cartas.size() < maxCartas) { // Verifica que la fila esté activa y hay espacio
            cartas.push_back(carta); // agrego la carta 
            return true;
        }
        return false;
    }

    void mostrarFila() const {
        if (activa) {
            for (const auto& carta : cartas) {  // carta es una referencia a cada elemento
                carta.mostrarCarta();
            }
        } else {
            cout << "Fila inactiva." << endl; // mensaje si activa es igual a false
        }
    }

    void desactivar() { activa = false; } // Método para desactivar la fila
    void activar() { activa = true; } // Método para activar la fila

    bool estaLlena() const {
        return cartas.size() >= maxCartas;
    }

    vector<Carta> obtenerCartas() {
        vector<Carta> cartasTemp = cartas; // creo una variable de tipo vector; un vector temporal para poder borrar el vector principal cuando arrastren
        cartas.clear();
        return cartasTemp;
    }
};




class Mazo {
private:
    vector<Carta> cartas;
    vector<string> colores = {"Rojo", "Azul", "Verde", "Amarillo", "Naranja", "Cafe", "Morado"};

public:
    Mazo() { // metodo Constructor
        crearCartasColores();
        crearCartasEspeciales();
        barajarMazo();
    }

    void crearCartasColores() { // creo la carta y los agrego al vector cartas
        for (const string& color : colores) {
            for (int i = 1; i <= 2; i++) {
                cartas.emplace_back(to_string(i), color);  // convertir numero a string
            }
        }
    }

    void crearCartasEspeciales() { // crea los comodines 
        for (int i = 0; i < 2; i++) {
            cartas.emplace_back("+2", "");
        }
        for (int i = 0; i < 2; i++) {
            cartas.emplace_back("Comodín", "");
        }
        cartas.emplace_back("Comodín Dorado", "");
        cartas.emplace_back("Última Ronda", "");
    }

    void barajarMazo() {
        random_device rd;
        mt19937 g(rd());
        shuffle(cartas.begin(), cartas.end(), g);
    }

    Carta jalarCarta() { // para saber si hay cartas en el mazo o no
        if (!cartas.empty()) {
            Carta carta = cartas.back();
            cartas.pop_back();
            return carta;
        }
        throw runtime_error("No hay más cartas en el mazo");
    }

    bool hayCartas() const {
        return !cartas.empty();
    }
};

class Juego {
private:
    Mazo mazo; // llamo a los elementos del mazo
    vector<Fila> filas;
    vector<vector<Carta>> areasDeTrabajo;
    vector<bool> jugadoresActivos;
    int jugadorActual;
    int numJugadores;
    bool ultimaRonda;
    int rondaActual;
    vector<set<string>> coloresSeleccionados;

public:
   Juego(int numJugadores)
    : numJugadores(numJugadores), jugadorActual(0), ultimaRonda(false), rondaActual(1), 
      coloresSeleccionados(numJugadores, set<string>()) { // Correcto aquí
    filas.resize(numJugadores); // cree las filas , las areas de trabajo , si el jugador arrastro una fila o no
    areasDeTrabajo.resize(numJugadores);
    jugadoresActivos.resize(numJugadores, true); 
}


    void jugarRonda() { // como inicia una ronda
    cout << "Iniciando Ronda " << rondaActual << endl;

    // Reiniciar el estado de las filas
    for (auto& fila : filas) {
        fila.activar(); // Activa todas las filas al inicio de la ronda
    }

    fill(jugadoresActivos.begin(), jugadoresActivos.end(), true); // lleno todos los jugadores con true nuevamente
    
    while (hayJugadoresActivos()) {
        jugarTurno();
        mostrarFilas();
    }
    
    cout << "Fin de la Ronda " << rondaActual << endl;
    rondaActual++;
}


    void jugarTurno() { // verifico si el jugador esta activo  para seguir
    if (!jugadoresActivos[jugadorActual]) { 
        avanzarTurno();
        return;
    }

    cout << "Turno del jugador " << (jugadorActual + 1) << ":" << endl;

    if (ultimaRonda) {
        cout << "¡Es la última ronda!" << endl;
    }

    int opcion; // variable para la opcion que eliga
    cout << "1. Jalar una carta\n2. Tomar una fila\nOpción: ";
    cin >> opcion;

    if (opcion == 1) {
        jalarYColocarCarta();
    } else if (opcion == 2) {
        tomarFila();
        jugadoresActivos[jugadorActual] = false; // Desactiva al jugador actual después de tomar la fila
    } else {
        cout << "Opción no válida. Intente de nuevo." << endl;
        return;
    }

    avanzarTurno();
}



   void jalarYColocarCarta() {
    try {
        Carta micarta = mazo.jalarCarta(); // llamo a la carta de pito carta
        cout << "Carta jalada: ";
        micarta.mostrarCarta();

        if (micarta.tipo == "Última Ronda") {
            ultimaRonda = true;
            cout << "La carta 'Última Ronda' se coloca a un lado como recordatorio." << endl;
            return;
        }

        int filaIndex; // numero de fila 
        while (true) {
            cout << "Seleccione la fila (0 a " << numJugadores - 1 << ") para colocar la carta: ";
            cin >> filaIndex;

            if (filaIndex >= 0 && filaIndex < numJugadores) {
                if (filas[filaIndex].esActiva()) { // Verificar si la fila está activa
                    if (filas[filaIndex].agregarCarta(micarta)) {
                        cout << "Carta colocada en la fila " << filaIndex << endl;
                        break;
                    } else {
                        cout << "No puedes colocar la carta en la fila " << filaIndex << ". Intenta en otra fila." << endl;
                    }
                } else {
                    cout << "La fila " << filaIndex << " está inactiva. Intenta en otra fila." << endl;
                }
            } else {
                cout << "Fila no válida. Intente de nuevo." << endl;
            }
        }
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }
}

void tomarFila() {
    while (true) { 
        int filaIndex;
        cout << "Seleccione la fila (0 a " << numJugadores - 1 << ") para tomar: ";
        cin >> filaIndex;

        if (filaIndex >= 0 && filaIndex < numJugadores) {
            if (filas[filaIndex].esActiva()) { // Verificar si la fila está activa
                vector<Carta> cartasTomadas = filas[filaIndex].obtenerCartas(); // llamo obtener cartas
                areasDeTrabajo[jugadorActual].insert(areasDeTrabajo[jugadorActual].end(), cartasTomadas.begin(), cartasTomadas.end());

                cout << "Cartas tomadas de la fila " << filaIndex << ":" << endl;
                for (const auto& carta : cartasTomadas) {
                    carta.mostrarCarta();
                }
                filas[filaIndex].desactivar(); // Desactivar la fila después de tomarla
                break; // Salir del bucle si se tomó una fila válida
            } else {
                cout << "La fila " << filaIndex << " está inactiva. Por favor, elige otra fila." << endl;
            }
        } else {
            cout << "Índice de fila no válido. Intente de nuevo." << endl;
        }
    }
}

    void avanzarTurno() { // verifico si todavia hay jugadores activos
    do {
        jugadorActual = (jugadorActual + 1) % numJugadores;
    } while (!jugadoresActivos[jugadorActual] && hayJugadoresActivos());
    
    mostrarAreasDeTrabajo();
}


    bool hayJugadoresActivos() const {
        return any_of(jugadoresActivos.begin(), jugadoresActivos.end(), [](bool activo) { return activo; });
    }

    bool juegoTerminado() const {
        return ultimaRonda && !hayJugadoresActivos();
    }

    void mostrarFilas() const {
        for (int i = 0; i < numJugadores; ++i) {
            cout << "Fila " << i << ":" << endl;
            filas[i].mostrarFila();
            cout << "-------------" << endl;
        }
    }

    void mostrarAreasDeTrabajo() const {
        for (int i = 0; i < numJugadores; ++i) {
            cout << "Área de trabajo del jugador " << (i + 1) << ":" << endl;
            for (const auto& carta : areasDeTrabajo[i]) {
                carta.mostrarCarta();
            }
            cout << "-------------" << endl;
        }
    }

  
  void seleccionarColores(int jugador) {
    // Mapa para contar las cartas de colores en el área de trabajo del jugador
    map<string, int> conteoColores;

    // Contar las cartas de colores en el área de trabajo del jugador
    for (const auto& carta : areasDeTrabajo[jugador]) {
        if (!carta.color.empty()) {
            conteoColores[carta.color]++;
        }
    }

    // Mostrar colores disponibles con su cantidad *
    cout << "Colores disponibles y cantidades:" << endl;
    for (const auto& par : conteoColores) {
        cout << par.first << ": " << par.second << endl;
    }

    set<string>& coloresSeleccionadosJugador = coloresSeleccionados[jugador]; // Modificación aquí
    int cantidadSeleccionada = 0;

    // Seleccionar colores
    while (cantidadSeleccionada < 3) {
        string colorSeleccionado;
        cout << "Elige un color (te quedan " << (3 - cantidadSeleccionada) << " por elegir): ";
        cin >> colorSeleccionado;

        if (conteoColores.count(colorSeleccionado) && conteoColores[colorSeleccionado] > 0) { // me permite buscar si ese color fue encontrado en el conteo
            if (coloresSeleccionadosJugador.find(colorSeleccionado) == coloresSeleccionadosJugador.end()) {
                coloresSeleccionadosJugador.insert(colorSeleccionado);
                cantidadSeleccionada++;
            } else {
                cout << "Ya seleccionaste ese color. Elige otro." << endl;
            }
        } else {
            cout << "Color no válido o no tienes cartas de ese color." << endl;
        }
    }

 
}

void calcularPuntuacion() {
    cout << "Calculando puntuación final..." << endl;

    // Estructura para almacenar la puntuación de cada jugador
    vector<int> puntuaciones(numJugadores, 0);

    // Calcular la puntuación para cada jugador
    for (int i = 0; i < numJugadores; ++i) {
        // Mapa para contar las cartas de colores en el área de trabajo del jugador
        map<string, int> conteoColores;

        // Contar las cartas de colores en el área de trabajo del jugador
        for (const auto& carta : areasDeTrabajo[i]) {
            if (!carta.color.empty()) {
                conteoColores[carta.color]++;
            }
        }

        // Sumar los puntos por colores seleccionados
        for (const auto& color : coloresSeleccionados[i]) { // Acceder a los colores seleccionados
            if (conteoColores.count(color)) {
                puntuaciones[i] += calcularPuntos(conteoColores[color]);
            }
        }

        // Restar los puntos por colores no seleccionados
        for (const auto& par : conteoColores) {
            if (coloresSeleccionados[i].find(par.first) == coloresSeleccionados[i].end()) { // Verificar si no está seleccionado
                puntuaciones[i] -= calcularPuntos(par.second);
            }
        }

        // Sumar puntos por comodines
        int cantidadComodines = count_if(areasDeTrabajo[i].begin(), areasDeTrabajo[i].end(), [](const Carta& carta) {
            return carta.tipo == "+2";
        });
        puntuaciones[i] += cantidadComodines * 2; // Cada +2 suma 2 puntos
    }

    // Mostrar resultados finales
    for (int i = 0; i < numJugadores; ++i) {
        cout << "Puntuación del jugador " << (i + 1) << ": " << puntuaciones[i] << endl;
    }

    // Determinar el ganador
    int maxPuntos = *max_element(puntuaciones.begin(), puntuaciones.end());
    vector<int> ganadores;

    for (int i = 0; i < numJugadores; ++i) {
        if (puntuaciones[i] == maxPuntos) {
            ganadores.push_back(i + 1);
        }
    }

    cout << "El(los) ganador(es): ";
    for (const auto& ganador : ganadores) {
        cout << "Jugador " << ganador << " ";
    }
    cout << endl;
}




// Función para calcular los puntos de acuerdo a la tabla
int calcularPuntos(int cantidad) {
    if (cantidad == 1) return 1;
    if (cantidad == 2) return 3;
    if (cantidad == 3) return 6;
    if (cantidad == 4) return 10;
    if (cantidad == 5) return 15;
    if (cantidad >= 6) return 21;
    return 0; // En caso de cantidad negativa
}


};

int main() {
    int numJugadores;
    cout << "Ingrese el número de jugadores: ";
    cin >> numJugadores;

    while (numJugadores < 2) {
        cout << "Por favor, ingrese al menos dos jugadores: ";
        cin >> numJugadores;
    }

    Juego juego(numJugadores);

    while (!juego.juegoTerminado()) {
        juego.jugarRonda();
        juego.mostrarAreasDeTrabajo();
    }

    juego.calcularPuntuacion();

    return 0;
}
