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

void dibujar(SDL_Renderer *renderer, const int   dibujo[][PIXELES_X_LADO], int oX, int oY){
    int offsetX = oX * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING); // Posicion X en pixeles
    int offsetY = oY * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING); // Posicion Y en pixeles

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

void mapaVacio(Casilla** mapa , int filas, int columnas){

    for(int y = 0; y < filas; y++)
    {
        for(int x = 0; x < columnas; x++)
        {
            mapa[y][x].estado = 0;
            mapa[y][x].presionada = false;
        }
    }
}

void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, int estadoExit, int** mapa, int filas)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    for (int i = 0; i < filas; i++) free(mapa[i]);
    free(mapa);
    exit(estadoExit);
}

int _ColocarMinas(int posMinas[][2], int fil,int* x, int* y)
{
        int repetido = 0;
        if(fil > 0)
        {
          for(int i = 0; i < fil; i++)
            if(posMinas[i][0] == *x && posMinas[i][1] == *y)
            {
                repetido = 1;
            }
        }
        return repetido;
}

void** CrearMatriz(int filas, int columnas, size_t tamElem)
{
    void** mapa = malloc(filas * sizeof(void*));
    if (!mapa)
    {
        puts("No hay suficiente memoria para las filas.");
        return NULL;
    }

    for (int i = 0; i < filas; i++)
    {
        mapa[i] = malloc(columnas * tamElem);

        if (!mapa[i])
        {
            puts("No hay suficiente memoria para una de las filas.");
            // Liberar filas anteriores
            for (int j = 0; j < i; j++) free(mapa[j]);
                free(mapa);
            return NULL;
        }
    }

    return mapa;
}

void mapaLlenar(Casilla** mapa , int filas , int columnas , int minas){

    int x, y;
    srand(time(NULL));
    for (int m = 0; m < minas; m++)
    {
        int repetido = 1;

        while(repetido == 1)
        {
          x = rand() % filas;
          y = rand() % columnas;
          if(mapa[x][y].estado != -1)
          {
            mapa[x][y].estado = -1;
            repetido = 0;
          }
        }

        //Sumar alrededro de la mina
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                if (dx == 0 && dy == 0)
                    continue;

                int nf = x + dx;
                int nc = y + dy;
                if (nf >= 0 && nf < filas && nc >= 0 && nc < columnas)
                {
                    if (mapa[nf][nc].estado != -1)
                    {
                         mapa[nf][nc].estado++;
                    }

                }
            }
        }
    }
}

