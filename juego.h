#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"

#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one : (n) == 2 ? two : (n) == 3 ? three : mine)
#define ERROR_ARCHIVO 20
#define ERROR_CONFIGURACION 21
#define EJECUCION_OK 0
#define ERROR_FUENTE 10
//TDA
typedef struct{
    int estado;
    bool presionada;
}Casilla;

typedef struct{
    int cantCasillasPresionadas;
    Casilla** mapa;
} Juego;

typedef struct {
    char nombre[50];
    int puntaje;
} Jugador;

typedef struct {
    int x;
    int y;
} Coord;

//Prototipos
void fondoColor(SDL_Renderer* renderer);
Coord interfaz(SDL_Renderer* renderer , int dimensionM);
bool casillaColocacion(SDL_Renderer* renderer , int fil , int col , Coord* picord);
void casillaEstado(SDL_Renderer* renderer, SDL_Window* window , Juego* mapa,int minasCord[][2],int minas , int filas , int columnas , int xGrilla , int yGrilla , int pXi , int pYi);
bool casillaBandera(SDL_Renderer* renderer, int xGrilla , int yGrilla , int pXi , int pYi);

Casilla** matrizCrear(size_t filas, size_t columnas, size_t tamElem);
void matrizDestruir(Casilla** mapa , size_t filas);
void mapaVacio(Casilla** mapa, int filas, int columnas);
void mapaLlenar(Casilla** mapa , int filas , int columnas , int minas , int minasCord[][2]);

#endif // JUEGO_H
