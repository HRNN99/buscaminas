#include <stdio.h>
#include "juego.h"
#include "estados.h"

//Funcion destinada a crear una matriz con memoria dinamica
void** matrizCrear(size_t filas, size_t columnas, size_t tamElem){

    void** mat = malloc(filas * sizeof(void*));
    if (!mat){

        puts("No hay suficiente memoria para las filas.");
        return NULL;
    }

    void** ult = mat + filas - 1;

    for (void** i = mat ; i <= ult ; i++){

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
void matrizDestruir(void** mat , size_t filas){

    void** ult = mat + filas - 1;

    for (void** i = mat ; i <= ult ; i++){
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

//Funcion que coloca todas las casillas sin valor
bool casillaColocacion(SDL_Renderer* renderer){
    int offsetX = 0; // Variable para coordenada X
    int offsetY = 0; // Variable para ccoordenada Y

    int x,y = 0;
    while(y < TAM_GRILLA){
        x=0;
        while(x < TAM_GRILLA){
            offsetX = x % TAM_GRILLA;
            offsetY = y % TAM_GRILLA;
            dibujar(renderer, square1, offsetX, offsetY);
            x++;
        }
        y++;
    }

    return true;
}

//Funcion que coloca estados en las casillas
void casillaEstado(SDL_Renderer* renderer , Juego* juego , int filas , int columnas , int xGrilla , int yGrilla){

    Casilla** mapa = juego->mapa;

    if(xGrilla < 0 || xGrilla >= columnas || yGrilla < 0 || yGrilla >= filas){
        return;
    }

    if(!mapa[yGrilla][xGrilla].presionada){

        mapa[yGrilla][xGrilla].presionada = true;
        juego->cantCasillasPresionadas +=1;
        //Se podria optimizar haciendo que square2 sea por ejemplo -2 en el mapa?
        if(mapa[yGrilla][xGrilla].estado != 0){
            dibujar(renderer , eleccion(mapa[yGrilla][xGrilla].estado) , xGrilla , yGrilla);
            return;
        }

        dibujar(renderer , square2 , xGrilla , yGrilla);

        casillaEstado(renderer , juego , filas , columnas , xGrilla , yGrilla-1);
        casillaEstado(renderer , juego , filas , columnas , xGrilla-1 , yGrilla);
        casillaEstado(renderer , juego , filas , columnas , xGrilla+1 , yGrilla);
        casillaEstado(renderer , juego , filas , columnas , xGrilla , yGrilla+1);

        casillaEstado(renderer , juego , filas , columnas , xGrilla-1 , yGrilla-1);
        casillaEstado(renderer , juego , filas , columnas , xGrilla+1 , yGrilla-1);
        casillaEstado(renderer , juego , filas , columnas , xGrilla-1 , yGrilla+1);
        casillaEstado(renderer , juego , filas , columnas , xGrilla+1 , yGrilla+1);
    }
}

//Funcion para colocar bandera
bool casillaBandera(SDL_Renderer* renderer, int xGrilla , int yGrilla){
    dibujar(renderer , flag , xGrilla , yGrilla);
    return true;
}
