#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <queue>

using namespace std;

struct state {
  int fil;
  int col;
  Orientacion brujula;
  bool tiene_bikini;
  bool tiene_zapatillas;
  bool bien_situado;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      current_state.bien_situado = false;
      current_state.fil=-1;
      current_state.col=-1;
      current_state.brujula=static_cast<Orientacion>(0);
      current_state.tiene_bikini = false;current_state.tiene_zapatillas=false;
      contadorRecarga=0;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:
  
  // Declarar aquí las variables de estado
  state current_state;
  Action last_action;
  bool girar_derecha;
  queue<Action> plan;
  int contadorRecarga;
};

void planeaHastaObjetivo(queue<Action>& plan, const int& objetivo, const state& st);

void rellenaMapa(const vector<unsigned char>& terreno, vector<vector<unsigned char>>& mapa, const state& st);

int encuentraElementoImportante(const vector<unsigned char>& terreno, const state& st, const int& bateria);

#endif