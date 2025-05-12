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

void _ReiniciarMapa(int* mapa, int filas, int columnas)
{
    for(int i = 0; i < filas; i++)
    {
        for(int j = 0; j < columnas; j++)
        {
            mapa[i * columnas + j] = 0;
        }
    }
}

void FinalizarSDL(SDL_Window *ventana, SDL_Renderer *renderer, int estadoExit, int* mapa)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
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

void CrearMapa(int* mapa, int minas, int filas, int columnas)
{
    srand(time(0));
    _ReiniciarMapa(mapa, filas, columnas);

    int posMinas[minas][2];
    for (size_t i = 0; i < minas; i++)
    {
        int repetido = 1;
        int x = rand() % 9;
        int y = rand() % 9;
        while(repetido == 1)
        {
            repetido = _ColocarMinas(posMinas, i, &x, &y);
            x = rand() % filas - 1;
            y = rand() % columnas - 1;
        }
            posMinas[i][0] = x;
            posMinas[i][1] = y;
        //mapa[y][x] = -1;
        // Colocacion de unos
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
                    mapa[nf * columnas + nc] += 1;
                }
            }
}

    }

    for (size_t i = 0; i < minas; i++)
    {
        mapa[posMinas[i][0] * columnas + posMinas[i][1]] = -1;
    }
}


