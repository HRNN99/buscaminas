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

void dibujar(SDL_Window *ventana, SDL_Renderer *renderer, const int[][PIXELES_X_LADO], int oX, int oY);
void borrarPantalla(SDL_Window *ventana, SDL_Renderer *renderer);

#endif // DIBUJOS_H_INCLUDED
