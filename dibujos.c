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
    {0,0,255,255}, //Azul

    {235,141,235,255},
    {120,120,120,255},
    {84,84,84,255},
    {48,48,48,255}
};

//Funcion que  dibuja pixel por pixel
void dibujar(SDL_Renderer *renderer , int pixeles , const int dibujo[][pixeles] , int gX , int gY , int pXi , int pYi){
    int pXf = (pXi * TAM_PIXEL) + gX * (pixeles * TAM_PIXEL); // Posicion X en pixeles
    int pYf = (pYi * TAM_PIXEL) + gY * (PIXELES_X_LADO * TAM_PIXEL); // Posicion Y en pixeles

    for (int y = 0; y < pixeles; y++){

        for (int x = 0; x < pixeles; x++){

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
void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *font, int estadoExit)
{
    TTF_CloseFont(font);
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

int renderizarTexto(TTF_Font *font, int size, const char *texto, int colorTexto, int colorFondo, SDL_Renderer *render, int x, int y)
{
    TTF_SetFontSize(font, size);
    SDL_Color sdlColor = colores[colorTexto];
    if(!(strlen(texto) > 0)){ //Evita el renderizado con cero caracteres
        return 1;
    }

    //Creo la textura del texto
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, texto, sdlColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(render, textSurface);

    // Creo un rectangulo por detras de las letras para evitar limpiar el render
    SDL_Rect rectFondo = {x, y, textSurface->w, textSurface->h};
    SDL_SetRenderDrawColor(render , colores[colorFondo].r , colores[colorFondo].g , colores[colorFondo].b , colores[colorFondo].a);
    SDL_RenderFillRect(render , &rectFondo);

    // Escribo el texto
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(render, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return 0;
}


