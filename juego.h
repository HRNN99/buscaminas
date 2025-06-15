#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"
#include <SDL2/SDL_mixer.h>
#include <time.h>


#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one   \
                                      : (n) == 2   ? two   \
                                      : (n) == 3   ? three \
                                      : (n) == -1  ? mine2 \
                                                   : mine)
#define eleccionBandera(n) ((n) == 0 ? square1 : (n) == 1 ? flag \
                                                          : symbol)
#define ERROR_ARCHIVO 20
#define ERROR_CONFIGURACION 21
#define EJECUCION_OK 0
#define ERROR_FUENTE 10
#define ERROR_SONIDO 11
#define ERROR_ELIMINACION_ARCHIVO 15
#define ERROR_RENOMBRE_ARCHIVO 16
#define ERROR_LINEA_LARGA 11
#define MAX_PUNTAJES 20

#define NOMBRE_VENTANA "Buscaminas"
// ESTADOS
typedef enum
{
    ESTADO_MENU,
    ESTADO_GANADO,
    ESTADO_JUGANDO,
    ESTADO_CARGAR,
    ESTADO_SALIENDO

} EstadoJuego;

// sistemas
typedef struct
{
    

} Sistema;

// TDA
typedef struct
{
    int estado;
    bool presionada;
    int estadoBandera;
} Casilla;

typedef struct
{
    char nombre[40];
    int puntos;
} Puntaje;

typedef struct
{
    bool iniciado;
    int cantCasillasPresionadas;
    int puntaje;
    int cantMinasEnInterfaz;
    int dimMapa;
    char nombreJugador[40];
    bool finPartida;
    time_t start_time;
    Casilla **mapa;
    Mix_Chunk *sonidoMina;
    Mix_Chunk *sonidoClick;
    Mix_Chunk *sonidoBandera;
    Mix_Chunk *sonidoCat;
    Mix_Chunk *sonidoPerder;
    Mix_Chunk *sonidoEnter;
    Mix_Chunk *sonidoFlecha;
    Mix_Music *musicaFondo;
    Mix_Music *musicaMenu;
} Juego;

typedef struct
{
    int y;
    int x;
} Coord;

typedef struct
{
    char tipoEvento[20];
    struct tm fechaHora;
    int coordXY[2];
} Log;

typedef struct
{
    const char *texto;
    SDL_Rect rect;
} MenuItem;

// punteros a funciones
typedef void (*EventoClick)(Juego *juego, int x, int y, Coord *minasCoord, int minas);

// Prototipos
void manejar_eventos_menu(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego, int *seleccion, const int menu_count);
void dibujar_menu(SDL_Renderer *renderer, SDL_Window *ventana, TTF_Font *font, const char *menu_items[], const int menu_count, int *seleccion);

void manejar_eventos_juego(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego, Coord *minasCoord, int minas, Coord *picords, Coord *rbutton);
void manejar_eventos_ganado(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego);

void fondoColor(SDL_Renderer *renderer);
void interfaz(SDL_Renderer *renderer, TTF_Font *font, Juego *juego, Coord *pcords, int dimensionM, Coord *rbutton);
void interfazGanado(SDL_Renderer *renderer, SDL_Window *ventana, TTF_Font *font, Juego *juego, Coord *pcords, int dimensionM, Coord *rbutton);

void casillaColocacion(Casilla **mapa, SDL_Renderer *renderer, int fil, int col, Coord *picord);

void casillaEstado(Juego *juego, Coord *minasCoord, int minas, int gX, int gY, bool chordClick);
void casillaBandera(Juego *juego, int gX, int gY);

Casilla **matrizCrear(size_t filas, size_t columnas, size_t tamElem);
void matrizDestruir(Casilla **mapa, size_t filas);
void mapaVacio(Casilla **mapa, int filas, int columnas);
void mapaLlenar(Casilla **mapa, int filas, int columnas, Coord *minasCoord, int minas);
void mapaImprimir(Casilla **mapa, int filas, int columnas);
void mapaReiniciar(SDL_Renderer *renderer, Coord *pcord, Juego *juego, int filas, int columnas, Coord *minasCoord, int minas);

FILE *abrirArchivo(const char *nombre, const char *modo);

void handlerClickIzquierdo(Juego *juego, int x, int y, Coord *minasCoord, int minas);
void handlerClickDerecho(Juego *juego, int x, int y, Coord *minasCoord, int minas);

void clickDoble(Juego *juego, int gX, int gY, Coord *minasCoord, int minas);
// Log
void setLog(Log *log, int coordX, int coordY, char tipoEvento[80]);
#endif // JUEGO_H
