#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <queue>

using namespace std;


struct state
{
  int fil;
  int col;
  Orientacion brujula;
  bool tiene_bikini;
  bool tiene_zapatillas;
  bool bien_situado;
  bool atrapado;
};

class ComportamientoJugador : public Comportamiento
{

public:
  ComportamientoJugador(unsigned int size) : Comportamiento(size)
  {
    // Constructor de la clase
    // Dar el valor inicial a las variables de estado
    current_state.bien_situado = false;
    current_state.fil = -1;
    current_state.col = -1;
    current_state.brujula = static_cast<Orientacion>(0);
    current_state.tiene_bikini = false;
    current_state.tiene_zapatillas = false;
    contadorRecarga = 0;
    contadorAtrapado = 0;

    for (int i=0; i<size; i++) 
      heatMap.push_back(vector<int>(size));
    for (int i=3; i<size-3; i++)
      for (int j=3; j<size-3; j++)
        heatMap[i][j] = 0;
    for (int i=0; i<3; i++) {
      for (int j=0; j<size; j++) {
        heatMap[i][j] = 1000000; heatMap[j][i]=1000000;
        heatMap[size-i-1][j] = 1000000; heatMap[j][size-i-1]=1000000;
      }
    }

    for (int i = 0; i < 8; ++i)
      casillasTerreno.push_back(vector<pair<int, int>>(16));

    // Norte
    vector<pair<int, int>> posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(-1, -1), pair<int, int>(-1, 0), pair<int, int>(-1, 1),
        pair<int, int>(-2, -2), pair<int, int>(-2, -1), pair<int, int>(-2, 0), pair<int, int>(-2, 1), pair<int, int>(-2, 2),
        pair<int, int>(-3, -3), pair<int, int>(-3, -2), pair<int, int>(-3, -1), pair<int, int>(-3, 0), pair<int, int>(-3, 1), pair<int, int>(-3, 2), pair<int, int>(-3, 3)};
    casillasTerreno[0] = posiciones;

    // Noreste
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(-1, 0), pair<int, int>(-1, 1), pair<int, int>(0, 1),
        pair<int, int>(-2, 0), pair<int, int>(-2, 1), pair<int, int>(-2, 2), pair<int, int>(-1, 2), pair<int, int>(0, 2),
        pair<int, int>(-3, 0), pair<int, int>(-3, 1), pair<int, int>(-3, 2), pair<int, int>(-3, 3), pair<int, int>(-2, 3), pair<int, int>(-1, 3), pair<int, int>(0, 3)};
    casillasTerreno[1] = posiciones;

    // Este
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(-1, 1), pair<int, int>(0, 1), pair<int, int>(1, 1),
        pair<int, int>(-2, 2), pair<int, int>(-1, 2), pair<int, int>(0, 2), pair<int, int>(1, 2), pair<int, int>(2, 2),
        pair<int, int>(-3, 3), pair<int, int>(-2, 3), pair<int, int>(-1, 3), pair<int, int>(0, 3), pair<int, int>(1, 3), pair<int, int>(2, 3), pair<int, int>(3, 3)};
    casillasTerreno[2] = posiciones;

    // Sureste
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(0, 1), pair<int, int>(1, 1), pair<int, int>(1, 0),
        pair<int, int>(0, 2), pair<int, int>(1, 2), pair<int, int>(2, 2), pair<int, int>(2, 1), pair<int, int>(2, 0),
        pair<int, int>(0, 3), pair<int, int>(1, 3), pair<int, int>(2, 3), pair<int, int>(3, 3), pair<int, int>(3, 2), pair<int, int>(3, 1), pair<int, int>(3, 0)};
    casillasTerreno[3] = posiciones;

    // Sur
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(1, 1), pair<int, int>(1, 0), pair<int, int>(1, -1),
        pair<int, int>(2, 2), pair<int, int>(2, 1), pair<int, int>(2, 0), pair<int, int>(2, -1), pair<int, int>(2, -2),
        pair<int, int>(3, 3), pair<int, int>(3, 2), pair<int, int>(3, 1), pair<int, int>(3, 0), pair<int, int>(3, -1), pair<int, int>(3, -2), pair<int, int>(3, -3)};
    casillasTerreno[4] = posiciones;

    // Sin hacer: Suroeste
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(1, 0), pair<int, int>(1, -1), pair<int, int>(0, -1),
        pair<int, int>(2, 0), pair<int, int>(2, -1), pair<int, int>(2, -2), pair<int, int>(1, -2), pair<int, int>(0, -2),
        pair<int, int>(3, 0), pair<int, int>(3, -1), pair<int, int>(3, -2), pair<int, int>(3, -3), pair<int, int>(2, -3), pair<int, int>(1, -3), pair<int, int>(0, -3)};
    casillasTerreno[5] = posiciones;

    // Oeste
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(1, -1), pair<int, int>(0, -1), pair<int, int>(-1, -1),
        pair<int, int>(2, -2), pair<int, int>(1, -2), pair<int, int>(0, -2), pair<int, int>(-1, -2), pair<int, int>(-2, -2),
        pair<int, int>(3, -3), pair<int, int>(2, -3), pair<int, int>(1, -3), pair<int, int>(0, -3), pair<int, int>(-1, -3), pair<int, int>(-2, -3), pair<int, int>(-3, -3)};
    casillasTerreno[6] = posiciones;

    // Sin hacer : Noroeste
    posiciones = {
        pair<int, int>(0, 0),
        pair<int, int>(0, -1), pair<int, int>(-1, -1), pair<int, int>(-1, 0),
        pair<int, int>(0, -2), pair<int, int>(-1, -2), pair<int, int>(-2, -2), pair<int, int>(-2, -1), pair<int, int>(-2, 0),
        pair<int, int>(0, -3), pair<int, int>(-1, -3), pair<int, int>(-2, -3), pair<int, int>(-3, -3), pair<int, int>(-3, -2), pair<int, int>(-3, -1), pair<int, int>(-3, 0)};
    casillasTerreno[7] = posiciones;
  }

  ComportamientoJugador(const ComportamientoJugador &comport) : Comportamiento(comport) {}
  ~ComportamientoJugador() {}

  Action think(Sensores sensores);
  int interact(Action accion, int valor);

private:
  // Declarar aquí las variables de estado
  state current_state;
  Action last_action;
  bool girar_derecha;
  queue<Action> plan;
  queue<Action> planSecundario;
  int contadorRecarga;
  int contadorAtrapado;
  vector<vector<pair<int, int>>> casillasTerreno;
  vector<vector<int>> heatMap;
};
int ladoMasFrioNoSituado(const vector<unsigned char>& terreno, const state& st);
int ladoMasFrio(const state& current_state, const vector<vector<int>>& heatMap, const vector<vector<pair<int,int>>>& casillasTerreno);

int casillaMasFria(state &st, const vector<vector<int>> &heatMap, const vector<vector<pair<int, int>>> &casillasTerreno);

int direccionDesconocida(const state &current_state, const vector<vector<unsigned char>> &mapaResultado);

void planeaHastaObjetivo(queue<Action> &plan, const int &objetivo, const state &st);

void rellenaMapa(const vector<unsigned char> &terreno, vector<vector<unsigned char>> &mapa, const state &st, const vector<vector<pair<int,int>>>& casillasTerreno, vector<vector<int>>& heatMap, const Sensores& sensores);

int encuentraElementoImportante(const vector<unsigned char> &terreno, state &st, const int &bateria);

#endif