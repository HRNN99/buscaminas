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
    {48,48,48,255},
    {255, 251, 0, 255}, // Dorado
    {245, 216, 0, 255}, // Sombra Dorado
    {204, 133, 0, 255}, // Bronce
    {150, 100, 0, 255}, // Bronce sombra

    {34,20,99,255}, //Purpura
    {120,2,40,255}, //Rojo obscuro
    {11,150,181,255}//Cian
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

//Funcion que dibuja pixel por pixel en relacion absoluta
void dibujarAbsoluto(SDL_Renderer *renderer , int pixeles , const int dibujo[][pixeles] , int gX , int gY, int escala){

    for (int y = 0; y < pixeles; y++){

        for (int x = 0; x < pixeles; x++){

            //Utilizo los componentes rgba del color elegido en la paleta
            SDL_SetRenderDrawColor(renderer,
                                   colores[dibujo[y][x]].r,
                                   colores[dibujo[y][x]].g,
                                   colores[dibujo[y][x]].b,
                                   colores[dibujo[y][x]].a);

            //Estructura para representar un rectangulo, utilizado para plasmar un pixel
            SDL_Rect pixel = {gX + x*escala , gY + y*escala , escala , escala};
            //Dibuja un rectangulo lleno en el renderer
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
}

int (*construirCoronaConColores(const int origen[][24], int fondo, int principal, int sombra))[24] {
    int (*copia)[24] = malloc(sizeof(int[24][24]));
    if (!copia) return NULL;

    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            switch (origen[i][j]) {
                case GS: copia[i][j] = fondo; break;
                case AD: copia[i][j] = principal; break;
                case DS: copia[i][j] = sombra; break;
                default: copia[i][j] = origen[i][j]; break;
            }
        }
    }
    return copia;
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

void rectanguloLlenoAbsoluto(SDL_Renderer *renderer , int color , const int gX , const int gY , int W , int H){

    SDL_Rect rect;
    rect.x = gX;
    rect.y = gY;
    rect.w = W;
    rect.h = H;

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

void marcoInvertido(SDL_Renderer* renderer , int X , int Y , int W , int H , int G){

    //Cortorno
    rectanguloLlenoAbsoluto(renderer , BB , X , Y , W , G); //TOP
    rectanguloLlenoAbsoluto(renderer , GF , X , (Y + H - G) , W , G); //BOTTOM
    rectanguloLlenoAbsoluto(renderer , BB , X , Y , G , H); //LEFT
    rectanguloLlenoAbsoluto(renderer , GF , (X + W - G) , Y , G , H); //RIGHT
}

//Funcion que finaliza el SDL
void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, TTF_Font *font, int estadoExit, FILE* archivoLog)
{
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    fclose(archivoLog);
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

    SDL_Color sdlColor = colores[colorTexto]; //Color de texto

    if(!(strlen(texto) > 0)){ //Evita el renderizado con cero caracteres
        return 1;
    }

    //Creo la textura del texto
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, texto, sdlColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(render, textSurface);

    SDL_SetRenderDrawColor(render , colores[colorFondo].r , colores[colorFondo].g , colores[colorFondo].b , colores[colorFondo].a);

    // Escribo el texto
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(render, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    return 0;
}


