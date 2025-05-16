#include "dibujos.h"
SDL_Color colores[] =
{
    {0, 0, 0, 255},       // N[0] - Negro
    {255, 255, 0, 255},   // Y[1] - Amarillo
    {255, 255, 255, 255},  // B[2] - Blanco
    {0, 150, 60, 255},          // V[3] - Verde
    {255, 0, 0, 255},          // R[4] - Rojo
    {0, 0, 0, 100}          // G[5] - Gris


/*
    Si necesitan agregar más colores, es necesario agregar en dibujos.h el #define correspondiente
    por ejemplo #define R 3, y acá agregar una fila más que diga {255,0,0,255}
    Nota: el cuarto campo de cada color corresponde a la transparencia: 0 es totalmente transparente, 255 es totalmente visible.
    Ese campo en este ejemplo se ignora, porque después lo pisamos con un valor aleatorio.
*/
};

void _ReiniciarMapa(int** mapa, int filas, int columnas)
{
    for(int i = 0; i < filas; i++)
    {
        for(int j = 0; j < columnas; j++)
        {
            mapa[i][j] = 0;
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



void dibujar(SDL_Window *ventana, SDL_Renderer *renderer, const int   dibujo[][PIXELES_X_LADO], int oX, int oY)
{
    int offsetX = oX * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
    int offsetY = oY * (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
    int transparencia = rand() & 255;

    for (int y = 0; y < PIXELES_X_LADO; y++)
    {
        for (int x = 0; x < PIXELES_X_LADO; x++)
        {
            SDL_SetRenderDrawColor(renderer,
                                   colores[dibujo[y][x]].r,
                                   colores[dibujo[y][x]].g,
                                   colores[dibujo[y][x]].b,
                                   colores[dibujo[y][x]].a);
            SDL_Rect pixel = {offsetX + TAM_PIXEL*x, offsetY + TAM_PIXEL*y, TAM_PIXEL, TAM_PIXEL};
            SDL_RenderFillRect(renderer, &pixel);
        }
    }
    SDL_RenderPresent(renderer);
}

void borrarPantalla(SDL_Window *ventana, SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_Rect pixel = {0, 0, 0, 0};
    SDL_RenderFillRect(renderer, &pixel);
    SDL_RenderPresent(renderer);
}

_ColocarMinas(int posMinas[][2], int fil,int* x, int* y)
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
        mapa[i] = malloc(columnas * tamElem);  // inicializa en cero
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

int** CrearMapa(int filas, int columnas, int minas, size_t tamElem)
{
    int** mapa = (int**)CrearMatriz(filas, columnas, tamElem);
    int x, y;
    _ReiniciarMapa(mapa, filas, columnas);
    srand(time(NULL));
    for (int m = 0; m < minas; m++)
    {
        int repetido = 1;

        while(repetido == 1)
        {
          x = rand() % filas;
          y = rand() % columnas;
          if(mapa[x][y] != -1)
          {
            mapa[x][y] = -1;
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
                    if (mapa[nf][nc] != -1)
                    {
                         mapa[nf][nc]++;
                    }

                }
            }
        }
    }
    return mapa;
}

