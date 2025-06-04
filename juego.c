#include <stdio.h>
#include "juego.h"
#include "estados.h"
#include "time.h"

//Funcion destinada a crear una matriz con memoria dinamica
Casilla** matrizCrear(size_t filas, size_t columnas, size_t tamElem){

    Casilla** mat = malloc(filas * sizeof(Casilla*));
    if (!mat){

        puts("No hay suficiente memoria para las filas.");
        return NULL;
    }

    Casilla** ult = mat + filas - 1;

    for (Casilla** i = mat ; i <= ult ; i++){

        *i = malloc(columnas * tamElem);

        if (!*i){
            puts("No hay suficiente memoria para alguna de las filas.");
            matrizDestruir(mat , (i - mat));
            return NULL;
        }
    }

    return mat;
}

//Funcion para liberar la memoria de la matriz creada
void matrizDestruir(Casilla** mat , size_t filas){

    Casilla** ult = mat + filas - 1;

    for (Casilla** i = mat ; i <= ult ; i++){
        free(*i);
    }

    free(mat);
}

//Funcion que inicializa el mapa de juego en valores por defecto
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

//Funcion que llena el mapa de juego con minas y aleda�os
void mapaLlenar(Casilla** mapa , int filas , int columnas , int minas , int minasCord[][2]){

    int x , y , m=0 ;
    srand(time(NULL));

    while(m < minas){

        x = rand() % columnas;
        y = rand() % filas;

        if(mapa[y][x].estado != -1){

            minasCord[m][0] = x;
            minasCord[m][1] = y;

            mapa[y][x].estado = -1;

            //Sumar alrededro de la mina
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

//Establecer el renderizado con un color base (negro)
void fondoColor(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0,0,0,255); //color
    SDL_RenderClear(renderer); //limpieza
    SDL_RenderPresent(renderer); //Aplicacion
}

void interfaz(SDL_Renderer* renderer, Coord* pcords , int dimensionM){

    int G=2;
    int pad = G*4;

    int anchoM = dimensionM * PIXELES_X_LADO + 4;
    int altoC = 28;
    int anchoI = anchoM + 16;
    int altoI = pad + altoC + pad + anchoM + pad;

    rectanguloLleno(renderer , GS , pcords->x , pcords->y , anchoI , altoI);

    marco(renderer , pcords->x , pcords->y , anchoI , altoI , G);

    pcords->x += pad; pcords->y += pad;
    marco(renderer , pcords->x , pcords->y , anchoM , altoC ,  G);

    pcords->x += 0; pcords->y += altoC + pad;
    marco(renderer , pcords->x , pcords->y , anchoM , anchoM , G);

    pcords->x += G; pcords->y += G;
}

//Funcion que coloca todas las casillas sin valor
bool casillaColocacion(SDL_Renderer* renderer , int fil , int col , Coord* picord){

    int gX = 0; // Variable para coordenada X
    int gY = 0; // Variable para ccoordenada Y

    int x,y = 0;
    while(y < fil){

        x=0;
        while(x < col){

            gX = x % col;
            gY = y % fil;
            dibujar(renderer , square1 , gX , gY , picord->x , picord->y);
            x++;
        }
        y++;
    }

    return true;
}

//Funcion que coloca estados en las casillas
void casillaEstado(SDL_Renderer* renderer , SDL_Window* window, Juego* juego, int minasCord[][2],int minas, int filas , int columnas , int gX , int gY , Coord* picords){

    Casilla** mapa = juego->mapa;

    int mX = 0;
    int mY = 0;
    if(gX < 0 || gX >= columnas || gY < 0 || gY >= filas){
        return;
    }

    if(!mapa[gY][gX].presionada){

        mapa[gY][gX].presionada = true;
        juego->cantCasillasPresionadas +=1;
        //Se podria optimizar haciendo que square2 sea por ejemplo -2 en el mapa?
        if(mapa[gY][gX].estado == -1){
            for(int i = 0; i<minas; i++)
            {
                mX = minasCord[i][0];
                mY = minasCord[i][1];
              dibujar(renderer , eleccion(mapa[gY][gX].estado) , mX , mY , picords->x , picords->y);
            }

        }else if(mapa[gY][gX].estado != 0){
            dibujar(renderer , eleccion(mapa[gY][gX].estado) , gX , gY , picords->x , picords->y);
            return;
        }

        dibujar(renderer , square2 , gX , gY , picords->x , picords->y);

        for(int i = -1; i<2; i++)
        {
            for(int j = -1; j<2; j++)
            {
                casillaEstado(renderer , window , juego, minasCord , minas , filas , columnas , gX + i , gY + j , picords);
            }
        }

    }
}

//Funcion para colocar bandera
void casillaBandera(SDL_Renderer* renderer, int xGrilla , int yGrilla , Coord* picord){
    dibujar(renderer , flag , xGrilla , yGrilla , picord->x , picord->y);
}
