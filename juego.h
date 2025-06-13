#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"
#include <time.h>

#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one : (n) == 2 ? two : (n) == 3 ? three : mine)
#define eleccionBandera(n) ((n) == 0 ? square1 : (n) == 1 ? flag : symbol)
#define ERROR_ARCHIVO 20
#define ERROR_CONFIGURACION 21
#define EJECUCION_OK 0
#define ERROR_FUENTE 10

//ESTADOS
typedef enum{
    ESTADO_MENU,
    ESTADO_JUGANDO,
    ESTADO_CARGAR,
    ESTADO_SALIENDO

}EstadoJuego;

//TDA
typedef struct{
    int estado;
    bool presionada;
    int estadoBandera;
}Casilla;

typedef struct{
    bool iniciado;
    int cantCasillasPresionadas;
    int puntaje;
    int cantMinasEnInterfaz;
    int dimMapa;
    bool finPartida;
    time_t start_time;
    Casilla** mapa;
} Juego;

typedef struct {
    int x;
    int y;
} Coord;

typedef struct{
    char tipoEvento[20];
    struct tm fechaHora;
    int coordXY[2];
} Log;

typedef struct {
    const char *texto;
    SDL_Rect rect;
} MenuItem;

//punteros a funciones
typedef void (*EventoClick)(Juego *juego, int x, int y, Coord *minasCoord, int minas);


//Prototipos
void manejar_eventos_menu(SDL_Event *e , EstadoJuego *estado_actual , int* seleccion , const int menu_count);
void dibujar_menu(SDL_Renderer* renderer , SDL_Window* ventana , TTF_Font* font , const char* menu_items[] , const int menu_count , int* seleccion);

void manejar_eventos_juego(SDL_Event *e , EstadoJuego *estado_actual , Juego* juego , Coord* minasCoord , int minas , Coord* picords , Coord* rbutton);


void fondoColor(SDL_Renderer* renderer);
void interfaz(SDL_Renderer* renderer, Coord* pcords , int dimensionM , Coord* rbutton);

void casillaColocacion(Casilla** mapa, SDL_Renderer* renderer , int fil , int col , Coord* picord);

void casillaEstado(Juego* juego , Coord* minasCoord , int minas , int gX , int gY, bool chordClick);
void casillaBandera(Juego *juego, int gX, int gY);

Casilla** matrizCrear(size_t filas, size_t columnas, size_t tamElem);
void matrizDestruir(Casilla** mapa , size_t filas);
void mapaVacio(Casilla** mapa, int filas, int columnas);
void mapaLlenar(Casilla** mapa , int filas , int columnas , Coord* minasCoord , int minas);
void mapaImprimir(Casilla** mapa, int filas, int columnas);
void mapaReiniciar(SDL_Renderer *renderer, Coord *pcord, Juego *juego, int filas, int columnas, Coord *minasCoord, int minas);


void handlerClickIzquierdo(Juego *juego, int x, int y, Coord *minasCoord, int minas);
void handlerClickDerecho(Juego *juego, int x, int y, Coord *minasCoord, int minas);



void clickDoble(Juego *juego, int gX, int gY, Coord *minasCoord, int minas);
//Log
void setLog(Log* log, int coordX, int coordY, char tipoEvento[80]);
#endif // JUEGO_H
