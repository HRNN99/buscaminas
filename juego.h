#ifndef JUEGO_H
#define JUEGO_H

#include "dibujos.h"
#include "sonido.h"
#include <SDL2/SDL_mixer.h>
#include <time.h>


#define eleccion(n) ((n) == 0 ? square2 : (n) == 1 ? one   \
                                      : (n) == 2   ? two   \
                                      : (n) == 3   ? three \
                                      : (n) == 4   ? four  \
                                      : (n) == 5   ? five  \
                                      : (n) == 6   ? six   \
                                      : (n) == 7   ? seven \
                                      : (n) == 8   ? eight \
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
#define MAX_SLOTS 3 //maximo de slots para guardado
#define ARCHIVO_PARTIDAS "partidas.dat"

//ESTADOS
typedef enum{
    ESTADO_MENU,
    ESTADO_DIFICULTAD,
    ESTADO_CARGAR,
    ESTADO_JUGANDO,
    ESTADO_GANADO,
    ESTADO_SALIENDO,
    ESTADO_PAUSA,
    ESTADO_GUARDAR

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
    int estadoBandera;
    bool presionada;
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
    bool finPartida;
    int cantCasillasPresionadas;
    int cantMinasEnMapa;
    Casilla mapa[100 * 100];
} JuegoHistorial;

typedef struct{
    bool iniciado;
    FILE *historial;
    int contadorHistorial;
    Dificultad dificultad;
    Casilla** mapa;
    Coord* minasCoord;
    int cantCasillasPresionadas;
    JuegoHistorial* juegoHistorial;
    int cantMinasEnMapa; // Minas mostradas en hud
    int puntaje; // Tiempo
    char nombreJugador[40];
    bool finPartida;
    Puntaje puntajes[MAX_PUNTAJES];
    int totalPuntajes;
    time_t start_time;
    int senialRender;
    FILE *log;
} Juego;


typedef struct{
    bool iniciado;
    Dificultad dificultad;
    int cantCasillasPresionadas;
    int puntaje;
    char nombreJugador[40];
    bool finPartida;
    Puntaje puntajes[MAX_PUNTAJES];
    int totalPuntajes;
    time_t start_time;
    int senialRender;
    Casilla mapa[100 * 100];
} JuegoGuardado;

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
    char tipoEvento[50];
    struct tm fechaHora;
    int coordXY[2];
} Log;

typedef struct{
    const char *texto;
    SDL_Rect rect;
} MenuItem;

// punteros a funciones
typedef void (*EventoClick)(Juego *juego, Sonido *sonidos, int x, int y);

//Prototipos
int manejar_eventos_menu(SDL_Event *e , EstadoJuego *estado_actual , int* seleccion , const int items_count , Sonido* sonidos);
int manejar_eventos_dificultad(Graficos *graficos, SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int items_count , Juego* juego , Dificultad* difs , SDL_Window* ventana);
int manejar_eventos_juego(Graficos *graficos,SDL_Event *e, EstadoJuego *estado_actual, EstadoJuego *estado_anterior, Juego *juego, Coord *picords, Coord *rbutton, Sonido *sonidos);
int manejar_eventos_ganado(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego);
void manejar_eventos_slots(Graficos *graficos, SDL_Event *e, EstadoJuego *estado_actual, Sonido *sonidos, int *seleccion, const int menu_count, Juego *juego);
void manejar_eventos_pausa(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego, int *seleccion, Sonido *sonidos, const int menu_count, Juego partidas[3]);
void interfazPausa(SDL_Renderer *renderer, SDL_Window *ventana, TTF_Font *font, Juego *juego, Coord *pcords, int dimensionM, Coord *rbutton, int *eleccion, bool musicaActiva, const char *menu_items[], int menu_count);


void fondoColor(SDL_Renderer* renderer);
void casillaColocacion(SDL_Renderer *renderer, Casilla **mapa , int dimension , Coord *picord);
void mapaReiniciar(Juego *juego);

void dibujar_menu(Graficos* graficos, const char *menu_items[], const int menu_count, int *seleccion , SDL_Texture* fondo);

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

FILE *abrirArchivo(const char *nombre, const char *modo, Juego* juego);

void handlerClickIzquierdo(Juego *juego, Sonido *sonidos, int x, int y);
void handlerClickDerecho(Juego *juego, Sonido *sonidos, int x, int y);

void clickDoble(Juego *juego , Sonido *sonidos ,  int gX , int gY);
// Log
void setLog(Log *log, int coordX, int coordY, char tipoEvento[80]);


//carga de partidas

void convertirAJuegoGuardado(Juego *orig, JuegoGuardado *dest);
void convertirAJuegoHistorial(Juego *orig, JuegoHistorial *dest);
void convertirAJuego(JuegoGuardado *src, Juego *dest);
void guardarPartidas(Juego partidas[3], const char *filename);
int cargarPartidas(Juego partidas[3], const char *filename);
void guardarHistorial(Juego* juego);
void cargarHistorialAtras(Juego* juego);
void cargarHistorialAdelante(Juego* juego);
bool archivoExiste(const char *filename);
void inicializarPartida(Juego* partidas);
void guardarEnSlot(Juego *juego, int slot);
int cargarDesdeSlot(Graficos *graficos, Juego *juego, int slot);

#endif // JUEGO_H
