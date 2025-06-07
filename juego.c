#include <stdio.h>
#include "juego.h"
#include "estados.h"
#include "time.h"

// Funcion destinada a crear una matriz con memoria dinamica
Casilla **matrizCrear(size_t filas, size_t columnas, size_t tamElem)
{

    Casilla **mat = malloc(filas * sizeof(Casilla *));
    if (!mat)
    {

        puts("No hay suficiente memoria para las filas.");
        return NULL;
    }

    Casilla **ult = mat + filas - 1;

    for (Casilla **i = mat; i <= ult; i++)
    {

        *i = malloc(columnas * tamElem);

        if (!*i)
        {
            puts("No hay suficiente memoria para alguna de las filas.");
            matrizDestruir(mat, (i - mat));
            return NULL;
        }
    }

    return mat;
}

// Funcion para liberar la memoria de la matriz creada
void matrizDestruir(Casilla **mat, size_t filas)
{

    Casilla **ult = mat + filas - 1;

    for (Casilla **i = mat; i <= ult; i++)
    {
        free(*i);
    }

    free(mat);
}

// Funcion que inicializa el mapa de juego en valores por defecto
void mapaVacio(Casilla **mapa, int filas, int columnas)
{

    for (int y = 0; y < filas; y++)
    {
        for (int x = 0; x < columnas; x++)
        {
            mapa[y][x].estado = 0;
            mapa[y][x].presionada = false;
        }
    }
}

// Funcion que llena el mapa de juego con minas y aleda�os
void mapaLlenar(Casilla **mapa, int filas, int columnas, Coord *minasCoord, int minas)
{

    int x, y, m = 0;
    srand(time(NULL));

    while (m < minas)
    {

        x = rand() % columnas;
        y = rand() % filas;

        if (mapa[y][x].estado != -1)
        {

            minasCoord[m].x = x;
            minasCoord[m].y = y;

            mapa[y][x].estado = -1;

            // Sumar alrededro de la mina
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (dy == 0 && dx == 0)
                        continue;

                    int nf = y + dy;
                    int nc = x + dx;

                    if ((nf >= 0 && nf < filas) && (nc >= 0 && nc < columnas) && (mapa[nf][nc].estado != -1))
                        mapa[nf][nc].estado++;
                }
            }

            m++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// Establecer el renderizado con un color base (negro)
void fondoColor(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // color
    SDL_RenderClear(renderer);                      // limpieza
    SDL_RenderPresent(renderer);                    // Aplicacion
}

void interfaz(SDL_Renderer *renderer, Coord *pcords, int dimensionM, Coord *rbutton)
{

    int G = 2; // Grosor
    int pad = G * 4;

    int anchoM = dimensionM * PIXELES_X_LADO + 4;
    int altoC = 28;
    int anchoI = anchoM + 16;
    int altoI = pad + altoC + pad + anchoM + pad;

    rectanguloLleno(renderer, GS, pcords->x, pcords->y, anchoI, altoI);

    marco(renderer, pcords->x, pcords->y, anchoI, altoI, G); // Exterior

    pcords->x += pad;
    pcords->y += pad;
    marco(renderer, pcords->x, pcords->y, anchoM, altoC, G); // Puntaje

    dibujar(renderer, PIXELES_X_LADO * 2, restart_button, 0, 0, (anchoM / 2) - 7, pcords->y);
    rbutton->x = ((anchoM / 2) - 7);
    rbutton->y = pcords->y;

    pcords->x += 0;
    pcords->y += altoC + pad;
    marco(renderer, pcords->x, pcords->y, anchoM, anchoM, G); // Mapa

    pcords->x += G;
    pcords->y += G;
}

void mapaReiniciar(SDL_Renderer *renderer, Coord *pcord, Juego *juego, int filas, int columnas, Coord *minasCoord, int minas)
{

    Casilla **mapa = juego->mapa;

    juego->puntaje = 0;
    juego->cantMinasEnInterfaz = minas;
    juego->finPartida = false;
    juego->cantCasillasPresionadas = 0;
    juego->start_time = time(NULL); // Iniciar el contador cuando inicia el juego

    mapaVacio(mapa, filas, columnas);

    mapaLlenar(mapa, filas, columnas, minasCoord, minas);

    casillaColocacion(mapa, renderer, filas, columnas, pcord);
}

// Funcion que coloca todas las casillas sin valor
bool casillaColocacion(Casilla **mapa, SDL_Renderer *renderer, int fil, int col, Coord *picord)
{

    int gX = 0; // Variable para coordenada X
    int gY = 0; // Variable para ccoordenada Y

    int x, y = 0;
    while (y < fil)
    {

        x = 0;
        while (x < col)
        {
            mapa[x][y].estadoBandera = 0; // Uso el ciclo para inicializar todo en 0
            gX = x % col;
            gY = y % fil;
            dibujar(renderer, PIXELES_X_LADO, square1, gX, gY, picord->x, picord->y);
            x++;
        }
        y++;
    }

    return true;
}

// Funcion que coloca estados en las casillas
void casillaEstado(SDL_Renderer *renderer, SDL_Window *window, Juego *juego, Coord *minasCoord, int minas, int filas, int columnas, int gX, int gY, Coord *picords)
{

    if (gX < 0 || gX >= columnas || gY < 0 || gY >= filas)
        return;

    Casilla *casillaSeleccionada = &juego->mapa[gY][gX]; //TODO: porque esta invertido?
    Casilla *casillaBandera = &juego->mapa[gX][gY]; 

    // No hacer nada si ya está presionada o tiene bandera
    if (casillaSeleccionada->presionada || casillaBandera->estadoBandera != 0)
        return;

    casillaSeleccionada->presionada = true;
    juego->cantCasillasPresionadas++;

    // Juego Perdido
    if (casillaSeleccionada->estado == -1)
    {
        juego->finPartida = true;
        dibujar(renderer, PIXELES_X_LADO, mine2, gX, gY, picords->x, picords->y);

        // Mostrar todas las bombas
        for (int i = 0; i < minas; i++)
        {

            int mX = minasCoord[i].x;
            int mY = minasCoord[i].y;

            if (gX != mX && gY != mY)
                dibujar(renderer, PIXELES_X_LADO, mine, mX, mY, picords->x, picords->y);
        }
        return;
    }

    // Dibuja numero y termina
    if (casillaSeleccionada->estado > 0)
    {
        dibujar(renderer, PIXELES_X_LADO, eleccion(casillaSeleccionada->estado), gX, gY, picords->x, picords->y);
        return;
    }

    dibujar(renderer, PIXELES_X_LADO, square2, gX, gY, picords->x, picords->y);

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0) continue; // Evita repetirse a sí mismo
            casillaEstado(renderer, window, juego, minasCoord, minas, filas, columnas, gX + i, gY + j, picords);
        }
    }
}

// Funcion para colocar bandera
void casillaBandera(SDL_Renderer *renderer, Juego *juego, int gX, int gY, Coord *picord, int* minasEnInterfaz)
{

    Casilla **mapa = juego->mapa;
    //Realizo una iteracion ciclica con el resto
    // 1%3 = 1, 2%3 = 2, 3%3 = 0;
    mapa[gX][gY].estadoBandera = (mapa[gX][gY].estadoBandera + 1) % 3;

    //Suma y resta de bombas dependiendo el caso
    if (mapa[gX][gY].estadoBandera == 1)
        (*minasEnInterfaz)--;
    else if (mapa[gX][gY].estadoBandera == 2)
        (*minasEnInterfaz)++;

    dibujar(renderer, PIXELES_X_LADO, eleccionBandera(mapa[gX][gY].estadoBandera), gX, gY, picord->x, picord->y);
}
