#include <stdio.h>
#include "juego.h"

const int square1[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int square2[10][10] =
{
    {GF,GF,GF,GF,GF,GF,GF,GF,GF,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {GF,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int mine[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,N,N,GS,GS,GS,GF},
    {B,GS,GS,N,N,N,N,GS,GS,GF},
    {B,GS,N,N,N,N,B,N,GS,GF},
    {B,GS,N,N,N,N,N,N,GS,GF},
    {B,GS,GS,N,N,N,N,GS,GS,GF},
    {B,GS,GS,GS,N,N,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int one[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,A,A,A,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,GS,A,A,GS,GS,GS,GF},
    {B,GS,GS,A,A,A,A,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int two[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,V,V,GS,GS,GS,GF},
    {B,GS,GS,V,GS,GS,V,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,V,GS,GS,GF},
    {B,GS,GS,GS,GS,V,GS,GS,GS,GF},
    {B,GS,GS,GS,V,GS,GS,GS,GS,GF},
    {B,GS,GS,V,V,V,V,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int three[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,R,R,R,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,R,GS,GS,GF},
    {B,GS,GS,GS,R,R,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,R,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,R,GS,GS,GF},
    {B,GS,GS,R,R,R,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

const int flag[10][10] =
{
    {B,B,B,B,B,B,B,B,B,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GS,GS,GS,R,N,GS,GS,GS,GF},
    {B,GS,GS,R,R,N,GS,GS,GS,GF},
    {B,GS,R,R,R,N,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,N,GS,GS,GS,GF},
    {B,GS,GS,GS,GS,N,GS,GS,GS,GF},
    {B,GS,GS,N,N,N,N,N,GS,GF},
    {B,GS,GS,GS,GS,GS,GS,GS,GS,GF},
    {B,GF,GF,GF,GF,GF,GF,GF,GF,GF}
};

/////////////////////////////////////////////////////////////////////////////

void fondoColor(SDL_Renderer* renderer){
    //Establecer el renderizado con un color base (negro)
    SDL_SetRenderDrawColor(renderer, 0,0,0,255); //color
    SDL_RenderClear(renderer); //limpieza
    SDL_RenderPresent(renderer); //Aplicacion
}

bool casillaColocacion(SDL_Renderer* renderer){
    int offsetX = 0; // Variable para coordenada X
    int offsetY = 0; // Variable para ccoordenada Y

    int x,y = 0;
    while(y < TAM_GRILLA){
        x=0;
        while(x < TAM_GRILLA){
            offsetX = x % TAM_GRILLA;
            offsetY = y % TAM_GRILLA;
            dibujar(renderer, square1, offsetX, offsetY);
            x++;
        }
        y++;
    }

    return true;
}

void casillaEstado(SDL_Renderer* renderer , Casilla** mapa , int filas , int columnas , int xGrilla , int yGrilla){

    if(xGrilla < 0 || xGrilla >= columnas || yGrilla < 0 || yGrilla >= filas){
        return;
    }

    if(!mapa[yGrilla][xGrilla].presionada){

        mapa[yGrilla][xGrilla].presionada = true;

        if(mapa[yGrilla][xGrilla].estado != 0){
            dibujar(renderer , eleccion(mapa[yGrilla][xGrilla].estado) , xGrilla , yGrilla);
            return;
        }

        dibujar(renderer , square2 , xGrilla , yGrilla);

        casillaEstado(renderer , mapa , filas , columnas , xGrilla , yGrilla-1);
        casillaEstado(renderer , mapa , filas , columnas , xGrilla-1 , yGrilla);
        casillaEstado(renderer , mapa , filas , columnas , xGrilla+1 , yGrilla);
        casillaEstado(renderer , mapa , filas , columnas , xGrilla , yGrilla+1);

        casillaEstado(renderer , mapa , filas , columnas , xGrilla-1 , yGrilla-1);
        casillaEstado(renderer , mapa , filas , columnas , xGrilla+1 , yGrilla-1);
        casillaEstado(renderer , mapa , filas , columnas , xGrilla-1 , yGrilla+1);
        casillaEstado(renderer , mapa , filas , columnas , xGrilla+1 , yGrilla+1);
    }
}

bool casillaBandera(SDL_Renderer* renderer, int xGrilla , int yGrilla){
    dibujar(renderer , flag , xGrilla , yGrilla);
    return true;
}
