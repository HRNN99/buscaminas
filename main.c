#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int main(int argc, char *argv[]){

    Casilla mapa[TAM_GRILLA][TAM_GRILLA] = {0,false}; // Matriz de 'Casillas' con tamaño TAM_GRILLA
    srand(time(0)); //Semilla rand

    // TEMPORAL - CREACION DE MAPA - MOVER
    int minasEnMapa = 8;
    int posMinas[minasEnMapa][2];
    for (size_t i = 0; i < minasEnMapa; i++){

        int x = rand() % 9;
        int y = rand() % 9;
        posMinas[i][0] = y;
        posMinas[i][1] = x;
        //mapa[y][x] = -1;
        // Colocacion de unos
        if (x - 1 >= 0)
        {
            mapa[y][x - 1].estado += 1;     // Izq
            if (y + 1 < 10)
            {
                mapa[y + 1][x - 1].estado += 1; // Diag inf iz
            }
            if (y - 1 >= 0)
            {
                mapa[y - 1][x - 1].estado += 1; // Diag sup iz
            }
        }

        if (x + 1 < 10)
        {
            mapa[y][x + 1].estado += 1;     // Der
            mapa[y + 1][x + 1].estado += 1; // Diag inf der
            mapa[y - 1][x + 1].estado += 1; // Diag sup der
        }

        if (y + 1 < 10)
        {
            mapa[y + 1][x].estado += 1; // Arriba
        }

        if (y - 1 >= 0)
        {
            mapa[y - 1][x].estado += 1; // Abajo
        }
    }

    for (size_t i = 0; i < minasEnMapa; i++){

        mapa[posMinas[i][0]][posMinas[i][1]].estado = -1;
    }

    //Iniciar SDL con funcion Video
    SDL_Init(SDL_INIT_VIDEO);
    char nombreVentana[100];
    //String formateado para el titulo de ventana
    sprintf(nombreVentana, "Buscaminas %ix%i", TAM_GRILLA, TAM_GRILLA);
    //Tamaño de ancho y altura de la ventana, utilizo 1 sola variable ya que sera cuadrada
    int TAM = TAM_GRILLA * TAM_PIXEL * PIXELES_X_LADO + TAM_GRILLA * PX_PADDING;
    //Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAM, TAM, 2);
    //Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer* renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    //Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // TEST ONLY // Grafico de matriz para debug
    for (size_t i = 0; i < TAM_GRILLA; i++){

        for (size_t j = 0; j < TAM_GRILLA; j++){

            printf("%3d ", mapa[i][j].estado);
        }
        printf("\n\n");
    }

    SDL_Event e; //Variable para registrar eventos
    int corriendo = 1; //Variable flag true para mantener corriendo el programa

    fondoColor(renderer);
    casillaColocacion(renderer);

    while (corriendo){ //While para mantener el programa corriendo

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)){//Registrando eventos

            if (e.type == SDL_QUIT){ //Evento de salida

                printf("Saliendo de SDL\n");
                corriendo = 0;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN){ //Se presiono una tecla del mouse

                int x = e.button.x; //Posicion X del click medido en pixeles
                int y = e.button.y; //Posicion Y del click medido en pixeles
                int boton = e.button.button; //Alias del boton presionado

                //Se dividen los pixeles para obtener un valor de grilla
                int xGrilla = e.button.x / (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
                int yGrilla = e.button.y / (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);

                if (boton == SDL_BUTTON_LEFT){ //Evento clik izquierdo del mouse
                    printf("Hiciste clic izquierdo en la casilla (%i,%i)\n", xGrilla , yGrilla);
                    casillaEstado(renderer , mapa , xGrilla , yGrilla);
                }

                else if (boton == SDL_BUTTON_RIGHT){ //Evento click derecho del mouse
                    printf("Hiciste clic derecho en la casilla (%i, %i) colocando bandera\n", xGrilla , yGrilla);
                    casillaBandera(renderer, xGrilla , yGrilla);
                }
            }

        }

        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }

    SDL_DestroyRenderer(renderer); //Funcion para destruir el renderer
    SDL_DestroyWindow(ventana); //Funcion para destrir la ventana
    SDL_Quit(); //Funcion para parar SDL init
    return 0;
}
