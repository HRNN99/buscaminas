#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dibujos.h"

const int fondo[8][8] ={
    {G, G, G, G, G, G, G, G},
    {G, B, B, B, B, B, B, G},
    {G, B, B, B, B, B, B, G},
    {G, B, B, B, B, B, B, G},
    {G, B, B, B, B, B, B, G},
    {G, B, B, B, B, B, B, G},
    {G, B, B, B, B, B, B, G},
    {G, G, G, G, G, G, G, G}
};

const int bomba[8][8] ={
    {B, B, B, N, N, B, B, B},
    {B, B, N, N, N, N, B, B},
    {B, N, N, B, N, N, N, B},
    {N, N, B, N, N, N, N, N},
    {N, N, N, N, N, N, N, N},
    {B, N, N, N, N, N, N, B},
    {B, B, N, N, N, N, B, B},
    {B, B, B, N, N, B, B, B}
};

const int bandera[8][8] ={
    {B, B, B, B, B, B, B, B},
    {B, B, B, N, R, R, R, B},
    {B, B, B, N, R, R, B, B},
    {B, B, B, N, R, B, B, B},
    {B, B, B, N, B, B, B, B},
    {B, B, B, N, B, B, B, B},
    {B, B, N, N, N, B, B, B},
    {B, N, N, N, N, N, B, B}

    };

int main(int argc, char* argv[])
{

    SDL_Init(SDL_INIT_VIDEO);
    char nombreVentana[100];
    sprintf(nombreVentana, "Tablero %dx%d",TAM_GRILLA,TAM_GRILLA);
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana,
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           TAM_GRILLA * TAM_PIXEL * PIXELES_X_LADO  + TAM_GRILLA * PX_PADDING,
                                           TAM_GRILLA * TAM_PIXEL * PIXELES_X_LADO  + TAM_GRILLA * PX_PADDING,
                                           2);

    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    borrarPantalla(ventana, renderer);
    srand(time(0));

    SDL_Event e;
    int corriendo = 1;
    int offsetX =  0;
    int offsetY = 0;

    while (corriendo)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                printf("Saliendo de SDL\n");
                corriendo = 0;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {

                offsetX = rand() % TAM_GRILLA;
                offsetY = rand() % TAM_GRILLA;

                int x = e.button.x;
                int y = e.button.y;

                int xGrilla = e.button.x / (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
                int yGrilla = e.button.y / (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);

                int boton = e.button.button;

                if (boton == SDL_BUTTON_LEFT)
                {
                    printf("Hiciste clic izquierdo en (%d, %d) poniendo un dibujo en la posición aleatoria [%d,%d]\n", x, y, xGrilla, yGrilla);
                    dibujar(ventana, renderer, bomba, xGrilla,  yGrilla);
                }
                else if (boton == SDL_BUTTON_RIGHT)
                {
                    printf("Hiciste clic derecho en (%d, %d)\n", x, y);
                    //borrarPantalla(ventana, renderer);
                    dibujar(ventana, renderer, bandera, xGrilla, yGrilla);
                }
            }
        }
        SDL_Delay(100);  // Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    return 0;
}
