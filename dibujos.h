#ifndef DIBUJOS_H_INCLUDED
#define DIBUJOS_H_INCLUDED

#include <SDL2/SDL.h> //Libreria especial SDL2
#include <stdio.h>
#include <stdbool.h>

#define TAM_GRILLA 10
#define TAM_PIXEL 3
#define PIXELES_X_LADO 10
#define PX_PADDING 1

#define NN 0 //Negro
#define BB 1 //Blanco
#define GS 2 //Gris Suave
#define GF 3 //Gris Fuerte
#define RR 4 //Rojo
#define VV 5 //Verde Oscuro
#define AA 6 //Azul

void dibujar(SDL_Renderer *renderer, const int[][PIXELES_X_LADO], int oX, int oY);
void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, int estadoExit, FILE* archivoLog);
void FinalizarVentanaSDL(SDL_Window *ventana, SDL_Renderer *renderer);

#endif // DIBUJOS_H_INCLUDED
