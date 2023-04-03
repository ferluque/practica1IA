#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void print_queue(queue<Action> q)
{
	while (!q.empty())
	{
		std::cout << q.front() << " ";
		q.pop();
	}
	std::cout << std::endl;
}

Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actFORWARD;
	indiceDefault = (indiceDefault+1)%maxForward;
	if (indiceDefault==0) {
		if ((random()%2)==0) {
			plan.push(actTURN_SL);plan.push(actTURN_SL);
		}
		else {
			plan.push(actTURN_SR);plan.push(actTURN_SR);
		}
	}
	tocaAleatorio = (random() % 3) != 0;
	int a;
	if (current_state.bien_situado)
	{
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
		}
	}
	if (sensores.nivel > 0)
	{
		cout << "Cambiamos orientación" << endl;
		switch (last_action)
		{
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
	switch (current_state.brujula)
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
	cout << "Orientación: " << current_state.brujula << endl;
	cout << "Bien situado: " << current_state.bien_situado << endl;
	// cout << "Atrapado: " << current_state.atrapado << endl;
	cout << "Longitud del plan: " << plan.size() << endl;
	cout << endl;

	// Si nos coge el lobo, ya no estaremos bien situados
	if (sensores.reset)
	{
		current_state.bien_situado = false;
		current_state.tiene_bikini = false;
		current_state.tiene_zapatillas = false;
	}

	// Nos situamos si estamos en una casilla azul
	if (((sensores.terreno[0] == 'G') or (sensores.nivel == 0)) and !current_state.bien_situado)
	{
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		current_state.bien_situado = true;
	}

	// Rellenar el mapa con la visión si estamos bien situados
	if (current_state.bien_situado)
		rellenaMapa(sensores.terreno, mapaResultado, current_state, casillasTerreno, heatMap, sensores);

	// Si llegamos a zapatillas o bikini los recogemos
	if (sensores.terreno[0] == 'K' && !current_state.tiene_bikini)
	{
		accion = actIDLE;
		current_state.tiene_bikini = true;
	}
	else if (sensores.terreno[0] == 'D' && !current_state.tiene_zapatillas)
	{
		accion = actIDLE;
		current_state.tiene_zapatillas = true;
	}
	// Vamos a recargar la batería al máximo siempre que estemos en una casilla de recarga
	// Como vemos, tiene prioridad máxima, pues si llegamos a una casilla de recarga siempre nos vamos a recargar enteros
	// Sin embargo, solo vamos a ir intencionadamente a una casilla de recarga cuando:
	// * Tengamos menos de 1000 de batería, con prioridad máxima
	// * Tengamos menos de 2500 de batería y no encontremos otra casilla de interés (bikini, zapatillas o posicionamiento) que siga siendo útil
	else if (((sensores.bateria < 3000 and !current_state.tiene_bikini and !current_state.tiene_zapatillas) or
			  (sensores.bateria < 2000 and !current_state.tiene_bikini and current_state.tiene_zapatillas) or
			  (sensores.bateria < 2000 and current_state.tiene_bikini and !current_state.tiene_zapatillas) or
			  (sensores.bateria < min(sensores.vida, 1500) and current_state.tiene_bikini and current_state.tiene_zapatillas)) &&
			 sensores.terreno[0] == 'X')
	{
		accion = actIDLE;
		contadorRecarga += 1;
		cout << "Recarga" << endl;
	}
	else if (plan.empty())
	{
		int objetivo = encuentraElementoImportante(sensores.terreno, current_state, sensores.bateria);
		if (objetivo != -1)
		{
			planeaHastaObjetivo(plan, objetivo, current_state);
			accion = plan.front();
			plan.pop();
		}
		if ((objetivo == -1) and !tocaAleatorio)
		{
			int miniObjetivo = ladoMasFrio(sensores.terreno, current_state, heatMap, casillasTerreno);
			planeaHastaObjetivo(plan, miniObjetivo, current_state);
			accion = plan.front();
			plan.pop();
			tocaAleatorio = (rand() % 3) != 0;
		}
	}
	else
	{
		accion = plan.front();
		plan.pop();
	}
	// Controlamos no tirarnos por el barranco ni chocarnos,  girando 135 grados y cancelando el plan
	if (accion == actFORWARD && (sensores.terreno[2] == 'P' or sensores.terreno[2] == 'M' or sensores.superficie[2] != '_'))
	{
		queue<Action> empty;
		swap(plan, empty);
		if (!girar_derecha)
		{
			accion = actTURN_BL;
			girar_derecha = (rand() % 2 == 0);
			plan.push(actFORWARD);
		}
		else
		{
			accion = actTURN_BR;
			girar_derecha = (rand() % 2 == 0);
			plan.push(actFORWARD);
		}
	}

	last_action = accion;
	return accion;
}

int ladoMasFrio(const vector<unsigned char> &terreno, const state &st, vector<vector<int>> &heatMap, const vector<vector<pair<int, int>>> &casillasTerreno)
{
	vector<double> valoraciones(terreno.size());
	for (int i = 0; i < valoraciones.size(); i++)
	{
		switch (terreno[i])
		{
		case 'B':
			if (st.tiene_zapatillas)
				valoraciones[i] = 2.0;
			else
				valoraciones[i] = 20.0;
			break;
		case 'A':
			if (st.tiene_bikini)
				valoraciones[i] = 3.0;
			else
				valoraciones[i] = 30.0;
			break;
		default:
			valoraciones[i] = 1.0;
			break;
		}
		if (st.bien_situado)
			valoraciones[i] += heatMap[st.fil + casillasTerreno[st.brujula][i].first][st.col + casillasTerreno[st.brujula][i].second];
	}
	double izq = (double)(valoraciones[1] + valoraciones[4] + valoraciones[5] + valoraciones[9] + valoraciones[10] + valoraciones[11]) / 6.0;
	double dcha = (double)(valoraciones[3] + valoraciones[7] + valoraciones[8] + valoraciones[13] + valoraciones[14] + valoraciones[15]) / 6.0;
	double medio = (double)(valoraciones[2] + valoraciones[6] + valoraciones[12]) / 3.0;

	if (izq < dcha and izq < medio)
		return 1;
	else if (medio <= dcha and medio <= izq)
		return 2;
	else if (dcha < medio and dcha < izq)
		return 3;
	return 2;
}

void giraHacia(queue<Action> &plan, const int objetivo, const state &st)
{
	int diferencia = objetivo - st.brujula;
	if ((diferencia <= 4 and diferencia > 0) or (diferencia <= -4 and diferencia > -8))
	{
		if (diferencia < 0)
			diferencia += 8;
		while (diferencia >= 3)
		{
			plan.push(actTURN_BR);
			diferencia -= 3;
		}
		while (diferencia > 0)
		{
			plan.push(actTURN_SR);
			diferencia--;
		}
	}
	else if (diferencia != 0)
	{
		if (diferencia > 0)
			diferencia -= 8;
		while (diferencia <= -3 and diferencia < 0)
		{
			plan.push(actTURN_BL);
			diferencia += 3;
		}
		while (diferencia < 0)
		{
			plan.push(actTURN_SL);
			diferencia++;
		}
	}
}

void planeaHastaObjetivo(queue<Action> &plan, const int &objetivo, const state &st)
{
	if ((st.brujula == 0 or st.brujula == 2 or st.brujula == 4 or st.brujula == 6) or
		(objetivo != 3 and objetivo != 8 and objetivo != 15 and objetivo != 1 and objetivo != 4 and objetivo != 9))
	{
		plan.push(actFORWARD);
		int posActual = 2;
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
			if (st.brujula == 0 or st.brujula == 2 or st.brujula == 4 or st.brujula == 6)
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
	else
	{
		if (objetivo == 1)
		{
			plan.push(actTURN_SL);
			plan.push(actFORWARD);
		}
		if (objetivo == 4)
		{
			plan.push(actTURN_SL);
			plan.push(actFORWARD);
			plan.push(actFORWARD);
		}
		if (objetivo == 9)
		{
			plan.push(actTURN_SL);
			plan.push(actFORWARD);
			plan.push(actFORWARD);
			plan.push(actFORWARD);
		}
		if (objetivo == 3)
		{
			plan.push(actTURN_SR);
			plan.push(actFORWARD);
		}
		if (objetivo == 8)
		{
			plan.push(actTURN_SR);
			plan.push(actFORWARD);
			plan.push(actFORWARD);
		}
		if (objetivo == 15)
		{
			plan.push(actTURN_SR);
			plan.push(actFORWARD);
			plan.push(actFORWARD);
			plan.push(actFORWARD);
		}
	}
	cout << "Plan: ";
	print_queue(plan);
}

int encuentraElementoImportante(const vector<unsigned char> &terreno, state &st, const int &bateria)
{
	int pos = -1;

	for (int i = 1; i < terreno.size(); i++)
	{
		if (st.atrapado and (terreno[i] == 'T' or terreno[i] == 'S' or terreno[i] == 'G' or terreno[i] == 'B' or terreno[i] == 'A'))
		{
			pos = i;
			st.atrapado = false;
		}
		// Si encuentra un hueco entre dos muros pasa por el porque seguramente sea interesante
		if (i == 2 or (i > 4 and i < 8) or (i > 9 and i < 15))
			if (terreno[i - 1] == 'M' and terreno[i] != 'M' and terreno[i] != 'P' and terreno[i + 1] == 'M')
				pos = i;
		if (terreno[i] == 'X' and bateria < 2500)
			pos = i;
		if (terreno[i] == 'K' && !st.tiene_bikini)
			pos = i;
		if (terreno[i] == 'D' && !st.tiene_zapatillas)
			pos = i;
		if (terreno[i] == 'G' && !st.bien_situado)
			pos = i;
		;
		if (terreno[i] == 'X' && bateria < 1500)
			pos = i;
	}
	if (st.brujula == 0 or st.brujula == 2 or st.brujula == 4 or st.brujula == 6)
	{
		// Otra forma de encontrar un hyeco
		if ((terreno[1] == 'M' or terreno[1] == 'P') and terreno[5] != 'M' and terreno[5] != 'P' and (terreno[11] == 'M' or terreno[11] == 'P'))
			pos = 5;
		else if ((terreno[2] == 'M' or terreno[2] == 'P') and terreno[6] != 'M' and terreno[6] != 'P' and (terreno[12] == 'M' or terreno[12] == 'P'))
			pos = 6;
		else if ((terreno[3] == 'M' or terreno[3] == 'P') and terreno[7] != 'M' and terreno[7] != 'P' and (terreno[13] == 'M' or terreno[13] == 'P'))
			pos = 7;
	}
	return pos;
}

void rellenaMapa(const vector<unsigned char> &terreno, vector<vector<unsigned char>> &mapa, const state &st, const vector<vector<pair<int, int>>> &casillasTerreno, vector<vector<int>> &heatMap, const Sensores &sensores)
{
	int i0 = st.fil, j0 = st.col;
	mapa[i0][j0] = terreno[0];
	heatMap[i0][j0]++;
	if (!st.tiene_bikini and mapa[i0][j0] == 'A')
		heatMap[i0][j0] += 50;
	else if (st.tiene_bikini and mapa[i0][j0] == 'A' and heatMap[i0][j0] > 50)
		heatMap[i0][j0] = 0.25 * sensores.vida;

	if (!st.tiene_zapatillas and mapa[i0][j0] == 'B')
		heatMap[i0][j0] += 20;
	else if (st.tiene_zapatillas and mapa[i0][j0] == 'B' and heatMap[i0][j0] > 20)
		heatMap[i0][j0] = 0.2 * sensores.vida;

	for (int i = 1; i < terreno.size(); ++i)
	{
		int fil = i0 + casillasTerreno[static_cast<int>(st.brujula)][i].first;
		int col = j0 + casillasTerreno[static_cast<int>(st.brujula)][i].second;
		mapa[fil][col] = terreno[i];

		if (!st.tiene_bikini and mapa[fil][col] == 'A')
			heatMap[fil][col] += 50;
		else if (st.tiene_bikini and mapa[fil][col] == 'A' and heatMap[fil][col] > 50)
			heatMap[fil][col] = 0.25 * sensores.vida;

		if (!st.tiene_zapatillas and mapa[fil][col] == 'B')
			heatMap[fil][col] += 20;
		else if (st.tiene_zapatillas and mapa[fil][col] == 'B' and heatMap[fil][col] > 20)
			heatMap[fil][col] = 0.2 * sensores.vida;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}