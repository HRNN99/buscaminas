#include "dibujos.h"

//Paleta
SDL_Color colores[] =
{
    {0,0,0,255}, //Negro
    {255,255,255,255}, //Blanco
    {203,203,203,255}, //Gris Suave
    {160,160,160,255}, //Gris Fuerte
    {255,0,0,255}, //Rojo
    {40,155,47,255}, //Verde oscuro
    {0,0,255,255} //Azul

};

//Funcion que  dibuja pixel por pixel
void dibujar(SDL_Renderer *renderer , const int dibujo[][PIXELES_X_LADO] , int gX , int gY , int pXi , int pYi){
    int pXf = (pXi * TAM_PIXEL) + gX * (PIXELES_X_LADO * TAM_PIXEL); // Posicion X en pixeles
    int pYf = (pYi * TAM_PIXEL) + gY * (PIXELES_X_LADO * TAM_PIXEL); // Posicion Y en pixeles

    for (int y = 0; y < PIXELES_X_LADO; y++){

        for (int x = 0; x < PIXELES_X_LADO; x++){

            //Utilizo los componentes rgba del color elegido en la paleta
            SDL_SetRenderDrawColor(renderer,
                                   colores[dibujo[y][x]].r,
                                   colores[dibujo[y][x]].g,
                                   colores[dibujo[y][x]].b,
                                   colores[dibujo[y][x]].a);

            //Estructura para representar un rectangulo, utilizado para plasmar un pixel
            SDL_Rect pixel = {pXf + TAM_PIXEL*x , pYf + TAM_PIXEL*y , TAM_PIXEL , TAM_PIXEL};
            //Dibuja un rectangulo lleno en el renderer
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
}

void rectanguloLleno(SDL_Renderer *renderer , int color , const int gX , const int gY , int W , int H){

    SDL_Rect rect;
    rect.x = gX * TAM_PIXEL;
    rect.y = gY * TAM_PIXEL;
    rect.w = W  * TAM_PIXEL;
    rect.h = H  * TAM_PIXEL;

    SDL_SetRenderDrawColor(renderer , colores[color].r , colores[color].g , colores[color].b , colores[color].a);
    SDL_RenderFillRect(renderer , &rect);
}

void marco(SDL_Renderer* renderer , int X , int Y , int W , int H , int G){

    //Cortorno
    rectanguloLleno(renderer , GF , X , Y , W , G); //TOP
    rectanguloLleno(renderer , BB , X , (Y + H - G) , W , G); //BOTTOM
    rectanguloLleno(renderer , GF , X , Y , G , H); //LEFT
    rectanguloLleno(renderer , BB , (X + W - G) , Y , G , H); //RIGHT
}

//Funcion que finaliza el SDL
void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, int estadoExit)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    exit(estadoExit);
}

//Funcion que finaliza una ventana
void FinalizarVentanaSDL(SDL_Window *ventana, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
}




