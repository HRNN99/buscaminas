#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"

#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one : (n) == 2 ? two : (n) == 3 ? three : mine)


//Prototipos
void fondoColor(SDL_Renderer* renderer);
bool casillaColocacion(SDL_Renderer* renderer);
void casillaEstado(SDL_Renderer* renderer , Casilla** mapa , int filas , int columnas , int xGrilla , int yGrilla);
bool casillaBandera(SDL_Renderer* renderer, int xGrilla , int yGrilla);

#endif // JUEGO_H
