#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"

#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one : (n) == 2 ? two : (n) == 3 ? three : mine)
#define eleccionBandera(n) ((n) == 0 ? square1 : (n) == 1 ? flag : symbol)
#define ERROR_ARCHIVO 20
#define ERROR_CONFIGURACION 21
#define EJECUCION_OK 0
#define ERROR_FUENTE 10
//TDA
typedef struct{
    int estado;
    bool presionada;
    int estadoBandera;
}Casilla;

typedef struct{
    int cantCasillasPresionadas;
    int puntaje;
    int cantMinasEnInterfaz;
    bool finPartida;
    time_t start_time;
    Casilla** mapa;
} Juego;

typedef struct {
    int x;
    int y;
} Coord;

//Prototipos
void fondoColor(SDL_Renderer* renderer);
void interfaz(SDL_Renderer* renderer, Coord* pcords , int dimensionM , Coord* rbutton);

bool casillaColocacion(Casilla** mapa, SDL_Renderer* renderer , int fil , int col , Coord* picord);
void casillaBandera(SDL_Renderer *renderer, Juego *juego, int gX, int gY, Coord *picord, int* minasEnInterfaz);

Casilla** matrizCrear(size_t filas, size_t columnas, size_t tamElem);
void matrizDestruir(Casilla** mapa , size_t filas);
void mapaVacio(Casilla** mapa, int filas, int columnas);
void mapaLlenar(Casilla** mapa , int filas , int columnas , Coord* minasCoord , int minas);
void casillaEstado(SDL_Renderer* renderer , SDL_Window* window, Juego* juego , Coord* minasCoord , int minas, int filas , int columnas , int gX , int gY , Coord* picords);
void mapaReiniciar(SDL_Renderer *renderer, Coord *pcord, Juego *juego, int filas, int columnas, Coord *minasCoord, int minas);

#endif // JUEGO_H
