#include "dibujos.h"

SDL_Color colores[] =
{
    {0,0,0,255}, //Negro
    {255,255,255,255}, //Blanco
    {203,203,203,255}, //Gris Suave
    {143,143,143,255}, //Gris Fuerte
    {255,0,0,255}, //Rojo
    {40,155,47,255}, //Verde oscuro
    {0,0,255,255} //Azul

};


void dibujar(SDL_Window *ventana, SDL_Renderer *renderer, const int   dibujo[][PIXELES_X_LADO], int oX, int oY)
{
    int offsetX = oX * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING); // Posicion X en pixeles
    int offsetY = oY * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING); // Posicion Y en pixeles
    int transparencia = rand() & 255;

    for (int y = 0; y < PIXELES_X_LADO; y++){

        for (int x = 0; x < PIXELES_X_LADO; x++){

            //Utilizo los componentes rgba del color elegido en la paleta
            SDL_SetRenderDrawColor(renderer,
                                   colores[dibujo[y][x]].r,
                                   colores[dibujo[y][x]].g,
                                   colores[dibujo[y][x]].b,
                                   colores[dibujo[y][x]].a);

            //Estructura para representar un rectangulo, utilizado para plasmar un pixel
            SDL_Rect pixel = {offsetX + TAM_PIXEL*x, offsetY + TAM_PIXEL*y, TAM_PIXEL, TAM_PIXEL};
            //Dibuja un rectangulo lleno en el renderer
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
}

