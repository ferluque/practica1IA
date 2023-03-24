#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{

	Action accion = actIDLE;
	int a;
	if (current_state.bien_situado) {
		switch (last_action)
		{
		case actFORWARD:
			switch (current_state.brujula)
			{
			case norte:
				current_state.fil--;
				break;
			case noreste:
				current_state.fil--;
				current_state.col++;
				break;
			case este:
				current_state.col++;
				break;
			case sureste:
				current_state.fil++;
				current_state.col++;
				break;
			case sur:
				current_state.fil++;
				break;
			case suroeste:
				current_state.fil++;
				current_state.col--;
				break;
			case oeste:
				current_state.col--;
				break;
			case noroeste:
				current_state.col--;
				current_state.fil--;
				break;
			}
			break;
		case actTURN_SL:
			a = current_state.brujula;
			a = (a + 7) % 8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_SR:
			a = current_state.brujula;
			a = (a + 1) % 8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_BL:
			a = current_state.brujula;
			a = (a + 5) % 8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_BR:
			a = current_state.brujula;
			a = (a + 3) % 8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		}
	}
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << " ";
	if (sensores.posF != -1)
	{
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
	}
	switch (sensores.sentido)
	{
	case 0:
		cout << "Norte" << endl;
		break;
	case 1:
		cout << "Noreste" << endl;
		break;
	case 2:
		cout << "Este" << endl;
		break;
	case 3:
		cout << "Sureste" << endl;
		break;
	case 4:
		cout << "Sur " << endl;
		break;
	case 5:
		cout << "Suroeste" << endl;
		break;
	case 6:
		cout << "Oeste" << endl;
		break;
	case 7:
		cout << "Noroeste" << endl;
		break;
	}
	cout << "Terreno: ";
	for (int i = 0; i < sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << endl;

	cout << "Superficie: ";
	for (int i = 0; i < sensores.superficie.size(); i++)
		cout << sensores.superficie[i];
	cout << endl;

	cout << "Colisión: " << sensores.colision << endl;
	cout << "Reset: " << sensores.reset << endl;
	cout << "Vida: " << sensores.vida << endl;
	cout << "Batería: " << sensores.bateria << endl;
	cout << "Contador Recarga: " << contadorRecarga << endl;
	cout << "Tiene bikini: " << current_state.tiene_bikini << endl;
	cout << "Tiene zapatillas: " << current_state.tiene_zapatillas << endl;
	cout << "Posicion: " << current_state.fil << ", " << current_state.col << endl;
	cout << "Bien situado: " << current_state.bien_situado << endl;
	cout << endl;

	// Nos situamos si estamos en una casilla azul
	if (sensores.terreno[0] == 'G' and !current_state.bien_situado)
	{
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		current_state.bien_situado = true;
	}
	// Si llegamos a zapatillas o bikini los recogemos
	if (sensores.terreno[0]=='K' && !current_state.tiene_bikini) {
		accion = actIDLE;
		current_state.tiene_bikini=true;
	}
	else if (sensores.terreno[0]=='D' && !current_state.tiene_zapatillas) {
		accion = actIDLE;
		current_state.tiene_zapatillas = true;
	}
	// Vamos a recargar la batería al máximo siempre que estemos en una casilla de recarga
	// Como vemos, tiene prioridad máxima, pues si llegamos a una casilla de recarga siempre nos vamos a recargar enteros
	// Sin embargo, solo vamos a ir intencionadamente a una casilla de recarga cuando:
	// * Tengamos menos de 1000 de batería, con prioridad máxima
	// * Tengamos menos de 2500 de batería y no encontremos otra casilla de interés (bikini, zapatillas o posicionamiento) que siga siendo útil
	else if (((sensores.bateria<3000 and !current_state.tiene_bikini and !current_state.tiene_zapatillas) or
	(sensores.bateria<2000 and !current_state.tiene_bikini and current_state.tiene_zapatillas) or
	(sensores.bateria<2000 and current_state.tiene_bikini and !current_state.tiene_zapatillas) or
	(sensores.bateria<min(sensores.vida, 1500) and current_state.tiene_bikini and current_state.tiene_zapatillas))
	&& sensores.terreno[0]=='X')
	{
		accion = actIDLE;
		contadorRecarga += 1;
		cout << "Recarga" << endl;
	}
	else
	{
		// Rellenar el mapa con la visión si estamos bien situados
		if (current_state.bien_situado)
			rellenaMapa(sensores.terreno, mapaResultado, current_state);

		// Planear hasta donde llegar si no hay plan y hay un punto interesante cerca
		if (plan.size() == 0)
		{
			// Calculamos si en nuestro rango de visión hay casillas desconocidas, en caso de que las haya, nos interesa ir a ellas
			// antes que a otras que ya conocemos
			int objetivo = encuentraElementoImportante(sensores.terreno, current_state, sensores.bateria);
			if (objetivo != -1)
			{
				planeaHastaObjetivo(plan, objetivo, current_state);
			}
		}

		// SIGUIENTE PASO: CONTROLAR COLISIONES, CAÍDAS...

		// Si tenemos plan, lo seguimos
		if (plan.size() != 0)
		{
			accion = plan.front();
			plan.pop();
			// Controlamos no tirarnos por el barranco ni chocarnos,  girando 135 grados y cancelando el plan
			if (accion == actFORWARD && (sensores.terreno[2] == 'P' or sensores.terreno[2]=='M'))
			{
				queue<Action> empty;
				swap(plan, empty);
				if (!girar_derecha)
				{
					accion = actTURN_BL;
					girar_derecha = (rand() % 2 == 0);
				}
				else
				{
					accion = actTURN_BR;
					girar_derecha = (rand() % 2 == 0);
				}
			}
		}
		else
		{
			// Si no tenemos plan, vemos en qué dirección no conocemos el terreno y avanzamos hacia allí
			
			if ((sensores.terreno[2] == 'T' or sensores.terreno[2] == 'S' or sensores.terreno[2] == 'G' or (sensores.terreno[2]=='B' && (sensores.bateria>1000 or current_state.tiene_zapatillas)) or (sensores.terreno[2]=='A') && (sensores.bateria>2000 or current_state.tiene_bikini)) and
				sensores.superficie[2] == '_')
			{
				accion = actFORWARD;
			}
			else if (!girar_derecha)
			{
				accion = actTURN_SL;
				girar_derecha = (rand() % 2 == 0);
			}
			else
			{
				accion = actTURN_SR;
				girar_derecha = (rand() % 2 == 0);
			}
		}
	}

	last_action = accion;
	return accion;
}

void planeaHastaObjetivo(queue<Action> &plan, const int &objetivo, const state &st)
{
	plan.push(actFORWARD);
	int posActual = 2;
	if (st.brujula == 0 or st.brujula == 2 or st.brujula == 4 or st.brujula == 6)
	{
		if (objetivo > 3)
		{
			plan.push(actFORWARD);
			posActual = 6;
			if (objetivo > 8)
			{
				plan.push(actFORWARD);
				posActual = 12;
			}
		}
		if (objetivo < posActual)
		{
			plan.push(actTURN_SL);
			plan.push(actTURN_SL);
		}
		if (objetivo > posActual)
		{
			plan.push(actTURN_SR);
			plan.push(actTURN_SR);
		}
		while (objetivo < posActual)
		{
			plan.push(actFORWARD);
			posActual--;
		}
		while (objetivo > posActual)
		{
			plan.push(actFORWARD);
			posActual++;
		}
	}
}

int encuentraElementoImportante(const vector<unsigned char> &terreno, const state &st, const int& bateria)
{
	int pos = -1;
	for (int i = 1; i < terreno.size() && pos == -1; i++)
	{
		// Intenta recargar la batería cuando tenga poca con menos prioridad
		if (terreno[i] == 'X' and bateria<2500)
			pos = i;
		if (terreno[i] == 'K' && !st.tiene_bikini)
			pos = i;
		if (terreno[i] == 'D' && !st.tiene_zapatillas)
			pos = i;
		if (terreno[i] == 'G' && !st.bien_situado)
			pos = i;
		if (terreno[i] == 'X' && bateria<1500)
			pos = i;
	}
	return pos;
}

void rellenaMapa(const vector<unsigned char> &terreno, vector<vector<unsigned char>> &mapa, const state &st)
{
	int i0 = st.fil, j0 = st.col;
	mapa[i0][j0] = terreno[0];
	int contador = 1;
	// Norte / Sur
	if (st.brujula == 0 or st.brujula == 4)
	{
		int factor = 1;
		if (st.brujula == 0)
			factor = -1;
		for (int i = 1; i < 4; ++i)
		{
			int m = 2 * i + 1;
			for (int j = 0; j < m; j++)
			{
				int fila = i0 + (factor * i);
				int columna = j0 + (factor * m / 2) - (factor * j);
				mapa[fila][columna] = terreno[contador];
				contador++;
			}
		}
	}
	// Este / Oeste
	if (st.brujula == 2 or st.brujula == 6)
	{
		int factor = 1;
		if (st.brujula == 6)
			factor = -1;
		for (int j = 1; j < 4; ++j)
		{
			int m = 2 * j + 1;
			for (int i = 0; i < m; i++)
			{
				int fila = i0 - (factor * m / 2) + (factor * i);
				int columna = j0 + (factor * j);
				mapa[fila][columna] = terreno[contador];
				contador++;
			}
		}
	}
	// Noreste / Suroeste
	if (st.brujula == 1)
	{
		mapa[i0 - 1][j0] = terreno[1];
		mapa[i0 - 1][j0 + 1] = terreno[2];
		mapa[i0][j0 + 1] = terreno[3];

		mapa[i0 - 2][j0] = terreno[4];
		mapa[i0 - 2][j0 + 1] = terreno[5];
		mapa[i0 - 2][j0 + 2] = terreno[6];
		mapa[i0 - 1][j0 + 2] = terreno[7];
		mapa[i0][j0 + 2] = terreno[8];

		mapa[i0 - 3][j0] = terreno[9];
		mapa[i0 - 3][j0 + 1] = terreno[10];
		mapa[i0 - 3][j0 + 2] = terreno[11];
		mapa[i0 - 3][j0 + 3] = terreno[12];
		mapa[i0 - 2][j0 + 3] = terreno[13];
		mapa[i0 - 1][j0 + 3] = terreno[14];
		mapa[i0][j0 + 3] = terreno[15];
	}
	// Sureste
	if (st.brujula == 3)
	{
		mapa[i0][j0 + 1] = terreno[1];
		mapa[i0 + 1][j0 + 1] = terreno[2];
		mapa[i0 + 1][j0] = terreno[3];

		mapa[i0][j0 + 2] = terreno[4];
		mapa[i0 + 1][j0 + 2] = terreno[5];
		mapa[i0 + 2][j0 + 2] = terreno[6];
		mapa[i0 + 2][j0 + 1] = terreno[7];
		mapa[i0 + 2][j0] = terreno[8];

		mapa[i0][j0 + 3] = terreno[9];
		mapa[i0 + 1][j0 + 3] = terreno[10];
		mapa[i0 + 2][j0 + 3] = terreno[11];
		mapa[i0 + 3][j0 + 3] = terreno[12];
		mapa[i0 + 3][j0 + 2] = terreno[13];
		mapa[i0 + 3][j0 + 1] = terreno[14];
		mapa[i0 + 3][j0] = terreno[15];
	}
	// Suroeste
	if (st.brujula == 5)
	{
		mapa[i0 + 1][j0] = terreno[1];
		mapa[i0 + 1][j0 - 1] = terreno[2];
		mapa[i0][j0 - 1] = terreno[3];

		mapa[i0 + 2][j0] = terreno[4];
		mapa[i0 + 2][j0 - 1] = terreno[5];
		mapa[i0 + 2][j0 - 2] = terreno[6];
		mapa[i0 + 1][j0 - 2] = terreno[7];
		mapa[i0][j0 - 2] = terreno[8];

		mapa[i0 + 3][j0] = terreno[9];
		mapa[i0 + 3][j0 - 1] = terreno[10];
		mapa[i0 + 3][j0 - 2] = terreno[11];
		mapa[i0 + 3][j0 - 3] = terreno[12];
		mapa[i0 + 2][j0 - 3] = terreno[13];
		mapa[i0 + 1][j0 - 3] = terreno[14];
		mapa[i0][j0 - 3] = terreno[15];
	}
	// Noroeste
	if (st.brujula == 7)
	{
		mapa[i0][j0 - 1] = terreno[1];
		mapa[i0 - 1][j0 - 1] = terreno[2];
		mapa[i0 - 1][j0] = terreno[3];

		mapa[i0][j0 - 2] = terreno[4];
		mapa[i0 - 1][j0 - 2] = terreno[5];
		mapa[i0 - 2][j0 - 2] = terreno[6];
		mapa[i0 - 2][j0 - 1] = terreno[7];
		mapa[i0 - 2][j0] = terreno[8];

		mapa[i0][j0 - 3] = terreno[9];
		mapa[i0 - 1][j0 - 3] = terreno[10];
		mapa[i0 - 2][j0 - 3] = terreno[11];
		mapa[i0 - 3][j0 - 3] = terreno[12];
		mapa[i0 - 3][j0 - 2] = terreno[13];
		mapa[i0 - 3][j0 - 1] = terreno[14];
		mapa[i0 - 3][j0] = terreno[15];
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}