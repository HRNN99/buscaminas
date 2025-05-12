#ifndef DIBUJOS_H_INCLUDED
#define DIBUJOS_H_INCLUDED
#include <SDL2/SDL.h>
#define TAM_PIXEL 10
#define PIXELES_X_LADO 8
#define TAM_GRILLA 10
#define PX_PADDING 4

#define N 0 // Negro
#define A 1 // Amarillo
#define B 2 // Blanco
#define V 3 // Verde
#define R 4 // Rojo
#define G 5 // Rojo

void dibujar(SDL_Window *ventana, SDL_Renderer *renderer, const int[][PIXELES_X_LADO], int oX, int oY);
void borrarPantalla(SDL_Window *ventana, SDL_Renderer *renderer);
void CrearMapa(int* mapa, int minas, int filas, int columnas);
void _ReiniciarMapa(int* mapa, int filas, int columnas);
int _ColocarMinas(int posMinas[][2], int fil,int* x, int* y);
void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, int estadoExit, int* mapa);
#endif // DIBUJOS_H_INCLUDED
