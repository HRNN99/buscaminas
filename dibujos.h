#ifndef DIBUJOS_H_INCLUDED
#define DIBUJOS_H_INCLUDED

#include <SDL2/SDL.h> //Libreria especial SDL2
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#define TAM_GRILLA 10
#define TAM_PIXEL 2
#define PIXELES_X_LADO 14

#define NN 0 //Negro
#define BB 1 //Blanco
#define GS 2 //Gris Suave
#define GF 3 //Gris Fuerte
#define RR 4 //Rojo
#define VV 5 //Verde Oscuro
#define AA 6 //Azul
#define RS 7//Rosa
#define GA 8//Gris A
#define GB 9//Gris B
#define GC 10//Gris C

void dibujar(SDL_Renderer *renderer , int pixeles , const int dibujo[][pixeles] , int gX , int gY , int pXi , int pYi);
void rectanguloLleno(SDL_Renderer *renderer , int color , const int gX , const int gY , int W , int H);
void marco(SDL_Renderer* renderer , int X , int Y , int W , int H , int G);
int renderizarTexto(TTF_Font *font, int size, const char *texto, int colorTexto, int colorFondo, SDL_Renderer *render, int x, int y);

void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *font, int estadoExit);
void FinalizarVentanaSDL(SDL_Window *ventana, SDL_Renderer *renderer);

#endif // DIBUJOS_H_INCLUDED
