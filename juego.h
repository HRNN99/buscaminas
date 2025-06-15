#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"
#include "sonido.h"
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
#define NOMBRE_VENTANA "Buscaminas"
#define MAX_PUNTAJES 5
#define TAMX_GANADO 220
#define TAMY_GANADO 276
#define TAM_BOTON_CERRADO 30

//ESTADOS
typedef enum{
    ESTADO_MENU,
    ESTADO_DIFICULTAD,
    ESTADO_CARGAR,
    ESTADO_JUGANDO,
    ESTADO_GANADO,
    ESTADO_SALIENDO

} EstadoJuego;

typedef struct {
    int x;
    int y;
} Coord;

// sistemas
typedef struct
{


} Sistema;

typedef struct{
    int estado;
    bool presionada;
    int estadoBandera;
} Casilla;

typedef struct{
    char nombre[20];
    int dimension;
    int cantidad_minas;
}Dificultad;

typedef struct{
    char nombre[40];
    int puntos;
} Puntaje;

typedef struct{
    bool iniciado;
    Dificultad dificultad;
    Casilla** mapa;
    Coord* minasCoord;
    int cantCasillasPresionadas;
    int puntaje;
    char nombreJugador[40];
    bool finPartida;
    Puntaje puntajes[MAX_PUNTAJES];
    int totalPuntajes;
    time_t start_time;
    int senialRender;
} Juego;

typedef struct
{
    Coord* piCord;
    SDL_Renderer *renderer;
    SDL_Window *ventana;
    int tamXVentana;
    int tamYVentana;
    TTF_Font *font;
    int G; // Grosor
    int pad; // Padding
    int altoC; // Alto boton reinicio
    int anchoM; // Ancho mapa
    int fontSize;
} Graficos;

typedef struct
{
    char tipoEvento[20];
    struct tm fechaHora;
    int coordXY[2];
} Log;

typedef struct{
    const char *texto;
    SDL_Rect rect;
} MenuItem;

// punteros a funciones
typedef void (*EventoClick)(Juego *juego, Sonido *sonidos, int x, int y, Coord *minasCoord, int minas);

//Prototipos
void manejar_eventos_menu(SDL_Event *e , EstadoJuego *estado_actual , int* seleccion , const int items_count , Sonido* sonidos);
void manejar_eventos_dificultad(Graficos *graficos, SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int items_count , Juego* juego , Dificultad* difs , SDL_Window* ventana);
void manejar_eventos_juego(SDL_Event *e , EstadoJuego *estado_actual , Juego* juego , Coord* picords , Coord* rbutton , Sonido *sonidos);
void manejar_eventos_ganado(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego);

void fondoColor(SDL_Renderer* renderer);
void casillaColocacion(SDL_Renderer *renderer, Casilla **mapa , int dimension , Coord *picord);
void mapaReiniciar(SDL_Renderer *renderer, Juego *juego);

void dibujar_menu(Graficos* graficos, const char *menu_items[], const int menu_count, int *seleccion);

void interfaz(Graficos* graficos, Juego *juego, Coord *rbutton);
void tiempoYbombas(Graficos* graficos, Juego *juego);
void interfazGanado(Graficos* graficos, Juego *juego);

void casillaEstado(Juego *juego , Sonido *sonidos , int gX, int gY, bool chordClick);
void casillaBandera(Juego *juego, int gX, int gY);

Casilla** matrizCrear(size_t filas, size_t columnas, size_t tamElem);
void mapaVacio(Casilla** mapa, int dimension);
void mapaLlenar(Casilla **mapa , int dimension , Coord *minasCoord , int minas);
void mapaImprimir(Casilla** mapa, int filas, int columnas);
void matrizDestruir(Casilla** mapa , size_t filas);

FILE *abrirArchivo(const char *nombre, const char *modo);

void handlerClickIzquierdo(Juego *juego, Sonido *sonidos, int x, int y);
void handlerClickDerecho(Juego *juego, Sonido *sonidos, int x, int y);

void clickDoble(Juego *juego , Sonido *sonidos ,  int gX , int gY);
// Log
void setLog(Log *log, int coordX, int coordY, char tipoEvento[80]);

#endif // JUEGO_H
