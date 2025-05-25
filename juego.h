#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"

#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one : (n) == 2 ? two : (n) == 3 ? three : mine)

//TDA
typedef struct{
    int estado;
    bool presionada;
}Casilla;

typedef struct{
    int cantCasillasPresionadas;
    Casilla** mapa;
} Juego;

//Prototipos
void fondoColor(SDL_Renderer* renderer);
bool casillaColocacion(SDL_Renderer* renderer);
void casillaEstado(SDL_Renderer* renderer , Juego* juego , int filas , int columnas , int xGrilla , int yGrilla);
bool casillaBandera(SDL_Renderer* renderer, int xGrilla , int yGrilla);

Casilla** matrizCrear(size_t filas, size_t columnas, size_t tamElem);
void matrizDestruir(Casilla** mapa , size_t filas);
void mapaVacio(Casilla** mapa, int filas, int columnas);
void mapaLlenar(Casilla** mapa , int filas , int columnas , int minas , int minasCord[][2]);

#endif // JUEGO_H
