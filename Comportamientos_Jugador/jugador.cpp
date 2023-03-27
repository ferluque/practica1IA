#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
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
	cout << "Orientación: " << current_state.brujula << endl;
	cout << "Bien situado: " << current_state.bien_situado << endl;
	// cout << "Atrapado: " << current_state.atrapado << endl;
	cout << "Longitud del plan: " << plan.size() << endl;
	cout << endl;

	// Si nos coge el lobo, ya no estaremos bien situados
	if (sensores.reset)
		current_state.bien_situado = false;

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
		rellenaMapa(sensores.terreno, mapaResultado, current_state, casillasTerreno, heatMap);

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
	else
	{
		// Planear hasta donde llegar si no hay plan y hay un punto interesante cerca
		if (plan.size() == 0)
		{
			// Calculamos si en nuestro rango de visión hay casillas desconocidas, en caso de que las haya, nos interesa ir a ellas
			// antes que a otras que ya conocemos
			int objetivo = encuentraElementoImportante(sensores.terreno, current_state, sensores.bateria);

			if (objetivo != -1)
			{
				cout << "Queremos ir a terreno: " << objetivo << endl;
				planeaHastaObjetivo(plan, objetivo, current_state);
				// Vaciamos ya el plan Secundario
				queue<Action> empty;
				swap(planSecundario, empty);
			}
		}

		// SIGUIENTE PASO: CONTROLAR COLISIONES, CAÍDAS...

		// Si tenemos plan, lo seguimos
		if (plan.size() != 0)
		{
			accion = plan.front();
			plan.pop();
		}
		else
		{
			// Si no tenemos plan, vemos en qué dirección no conocemos el terreno y avanzamos hacia allí
			// Si miramos a los puntos cardinales principales:
			int oriObjetivo = -1;
			// if ((current_state.brujula == 0 or current_state.brujula == 2 or current_state.brujula == 4 or current_state.brujula == 6) and current_state.bien_situado)
			// {
			// 	oriObjetivo = direccionDesconocida(current_state, mapaResultado);
			// 	if (oriObjetivo != -1)
			// 	{
			// 		// Caso girar a a la derecha
			// 		if (((oriObjetivo - current_state.brujula) == 2) or ((oriObjetivo - current_state.brujula) == -6))
			// 		{
			// 			plan.push(actTURN_SR);
			// 			plan.push(actTURN_SR);
			// 			plan.push(actFORWARD);
			// 		}
			// 		// Caso girar a a la izquierda
			// 		if (((oriObjetivo - current_state.brujula) == 6) or ((oriObjetivo - current_state.brujula) == -2))
			// 		{
			// 			plan.push(actTURN_SL);
			// 			plan.push(actTURN_SL);
			// 			plan.push(actFORWARD);
			// 		}
			// 		// Caso girar 180º
			// 		if (((oriObjetivo - current_state.brujula) == 4) or ((oriObjetivo - current_state.brujula) == -4))
			// 		{
			// 			plan.push(actTURN_BR);
			// 			plan.push(actTURN_SR);
			// 			plan.push(actFORWARD);
			// 		}
			// 	}
			// }

			if (oriObjetivo == -1 and current_state.bien_situado)
			{
				if (planSecundario.empty())
				{
					// Buscamos la casilla más fría y nos movemos ahí
					int posFria = casillaMasFria(current_state, heatMap, casillasTerreno);
					cout << "Queremos ir hasta " << posFria << " por temperatura" << endl;
					planeaHastaObjetivo(planSecundario, posFria, current_state);
				}
				accion = planSecundario.front();
				planSecundario.pop();
			}
			else if ((sensores.terreno[2] == 'T' or sensores.terreno[2] == 'S' or sensores.terreno[2] == 'G' or (sensores.terreno[2] == 'B' && (sensores.bateria > 500 or current_state.tiene_zapatillas)) or (sensores.terreno[2] == 'A') && (sensores.bateria > 1000 or current_state.tiene_bikini)) and
					 sensores.superficie[2] == '_')
			{
				accion = actFORWARD;
			}
			else if (!girar_derecha)
			{
				accion = actTURN_SL;
				girar_derecha = (rand() % 2 == 0);
				// Si vemos un muro, es probable que estemos atrapados
				if (sensores.terreno[2] == 'M')
				{
					contadorAtrapado++;
					if (contadorAtrapado > 20)
						current_state.atrapado = true;
				}
			}
			else
			{
				accion = actTURN_SR;
				girar_derecha = (rand() % 2 == 0);
				// Si vemos un muro, es probable que estemos atrapados
				if (sensores.terreno[2] == 'M')
				{
					contadorAtrapado++;
					if (contadorAtrapado > 20)
						current_state.atrapado = true;
				}
			}
		}
	}
	// Controlamos no tirarnos por el barranco ni chocarnos,  girando 135 grados y cancelando el plan
	if (accion == actFORWARD && (sensores.terreno[2] == 'P' or sensores.terreno[2] == 'M' or sensores.superficie[2] != '_'))
	{
		// Si vemos un muro, es probable que estemos atrapados
		if (sensores.terreno[2] == 'M')
		{
			contadorAtrapado++;
			if (contadorAtrapado > 20)
				current_state.atrapado = true;
		}
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
		cout << "Huimos" << endl;
	}

	last_action = accion;
	return accion;
}

int direccionDesconocida(const state &current_state, const vector<vector<unsigned char>> &mapaResultado)
{
	// Miramos en todas las direcciones y nos quedamos con la última que encontremos
	// Nota: Nunca vamos a elegir en este caso la dirección que estamos mirando, porque ya hemos actualizado mapaResultado
	int objetivo = -1;
	if (mapaResultado[current_state.fil - 1][current_state.col] == '?')
		objetivo = 0;
	if (mapaResultado[current_state.fil + 1][current_state.col] == '?')
		objetivo = 4;
	if (mapaResultado[current_state.fil][current_state.col + 1] == '?')
		objetivo = 2;
	if (mapaResultado[current_state.fil][current_state.col - 1] == '?')
		objetivo = 6;
	return objetivo;
}

void planeaHastaObjetivo(queue<Action> &plan, const int &objetivo, const state &st)
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
		{
			plan.push(actTURN_SL);
			plan.push(actTURN_SL);
		}
		else
			plan.push(actTURN_BL);
	}
	if (objetivo > posActual)
	{
		if (st.brujula == 0 or st.brujula == 2 or st.brujula == 4 or st.brujula == 6)
		{
			plan.push(actTURN_SR);
			plan.push(actTURN_SR);
		}
		else
			plan.push(actTURN_BR);
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

int casillaMasFria(state &st, const vector<vector<int>> &heatMap, const vector<vector<pair<int, int>>> &casillasTerreno)
{
	int minHeat = -1, posMinHeat = -1;

	minHeat = heatMap[st.fil][st.col];
	posMinHeat = 0;
	for (int i = 0; i < 16; i++)
	{
		int fil = st.fil + casillasTerreno[st.brujula][i].first;
		int col = st.col + casillasTerreno[st.brujula][i].second;
		if (heatMap[fil][col] < minHeat)
		{
			minHeat = heatMap[fil][col];
			posMinHeat = i;
		}
	}
	return posMinHeat;
}

int encuentraElementoImportante(const vector<unsigned char> &terreno, state &st, const int &bateria)
{
	int pos = -1;
	bool quiereSalir = false;

	for (int i = 1; i < terreno.size() && !quiereSalir; i++)
	{
		// Busca hueco entre dos muros, pero tiene que hacerlo de otra forma
		// if ((i==2) or (i>4 and i<8) or (i>9 and i<15)) {
		// 	if ((terreno[i-1]=='M' or terreno[i-1]=='P') and ((terreno[i+1]=='M' or terreno[i+1]=='P')) and
		// 	(terreno[i] == 'T' or terreno[i] == 'S' or terreno[i] == 'G' or terreno[i] == 'B' or terreno[i] == 'A')
		// 	) {
		// 		pos = i;quiereSalir=true;
		// 		cout << "Encuentro la salida en Terreno: " << pos << endl;
		// 	}
		// }
		// if (st.atrapado and (terreno[i] == 'T' or terreno[i] == 'S' or terreno[i] == 'G' or terreno[i] == 'B' or terreno[i]=='A')) {
		// 	pos = i;
		// 	st.atrapado=false;
		// }

		// Intenta recargar la batería cuando tenga poca con menos prioridad
		if (terreno[i] == 'X' and bateria < 2500)
			pos = i;
		if (terreno[i] == 'K' && !st.tiene_bikini)
			pos = i;
		if (terreno[i] == 'D' && !st.tiene_zapatillas)
			pos = i;
		if (terreno[i] == 'G' && !st.bien_situado)
			pos = i;
		if (terreno[i] == 'X' && bateria < 1500)
			pos = i;
	}
	return pos;
}

void rellenaMapa(const vector<unsigned char> &terreno, vector<vector<unsigned char>> &mapa, const state &st, const vector<vector<pair<int, int>>> &casillasTerreno, vector<vector<int>> &heatMap)
{
	int i0 = st.fil, j0 = st.col;
	mapa[i0][j0] = terreno[0];
	heatMap[i0][j0]++;
	cout << "Temperaturas: ";
	for (int i = 1; i < terreno.size(); ++i)
	{
		int fil = i0 + casillasTerreno[static_cast<int>(st.brujula)][i].first;
		int col = j0 + casillasTerreno[static_cast<int>(st.brujula)][i].second;
		mapa[fil][col] = terreno[i];
		cout << heatMap[fil][col];
	}
	cout << endl;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}