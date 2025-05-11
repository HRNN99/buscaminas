#ifndef DIBUJOS_H_INCLUDED
#define DIBUJOS_H_INCLUDED

#include <SDL2/SDL.h> //Libreria especial SDL2
#include <stdio.h>
#include <stdbool.h>

#define TAM_GRILLA 10
#define TAM_PIXEL 3
#define PIXELES_X_LADO 10
#define PX_PADDING 1

#define N 0 //Negro
#define B 1 //Blanco
#define GS 2 //Gris Suave
#define GF 3 //Gris Fuerte
#define R 4 //Rojo
#define V 5 //Verde Oscuro
#define A 6 //Azul

typedef struct{
    int estado;
    bool presionada;
}Casilla;

void dibujar(SDL_Renderer *renderer, const int[][PIXELES_X_LADO], int oX, int oY);
void despejar_recursivo(SDL_Window *ventana, SDL_Renderer *renderer,Casilla mapa[][TAM_GRILLA],int x, int y);

#endif // DIBUJOS_H_INCLUDED
