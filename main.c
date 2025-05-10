#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dibujos.h"
#include "estados.h" //Header de estados

int main(int argc, char *argv[]){

    int mapa[TAM_GRILLA][TAM_GRILLA] = {0}; // Matriz con el tamaño de grilla inicializada en 0
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
            mapa[y][x - 1] += 1;     // Izq
            if (y + 1 < 10)
            {
                mapa[y + 1][x - 1] += 1; // Diag inf iz
            }
            if (y - 1 >= 0)
            {
                mapa[y - 1][x - 1] += 1; // Diag sup iz
            }
        }

        if (x + 1 < 10)
        {
            mapa[y][x + 1] += 1;     // Der
            mapa[y + 1][x + 1] += 1; // Diag inf der
            mapa[y - 1][x + 1] += 1; // Diag sup der
        }

        if (y + 1 < 10)
        {
            mapa[y + 1][x] += 1; // Arriba
        }

        if (y - 1 >= 0)
        {
            mapa[y - 1][x] += 1; // Abajo
        }
    }

    for (size_t i = 0; i < minasEnMapa; i++){

        mapa[posMinas[i][0]][posMinas[i][1]] = -1;
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
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    //Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // TEST ONLY // Grafico de matriz para debug
    for (size_t i = 0; i < TAM_GRILLA; i++){

        for (size_t j = 0; j < TAM_GRILLA; j++){

            printf("%3d ", mapa[i][j]);
        }
        printf("\n\n");
    }

    SDL_Event e; //Variable para registrar eventos
    int corriendo = 1; //Variable flag true para mantener corriendo el programa
    int offsetX = 0; // Variable para coordenada X
    int offsetY = 0; // Variable para ccoordenada Y

    //Establecer el renderizado con un color base (negro)
    SDL_SetRenderDrawColor(renderer, 0,0,0,255); //color
    SDL_RenderClear(renderer); //limpieza

    int x,y = 0;
    while (corriendo){ //While para mantener el programa corriendo

        while(y < TAM_GRILLA){
            x=0;
            while(x < TAM_GRILLA){
                offsetX = x % TAM_GRILLA;
                offsetY = y % TAM_GRILLA;
                dibujar(ventana,renderer, square1, offsetX, offsetY);
                x++;
            }
            y++;
        }

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

                if (boton == SDL_BUTTON_LEFT) //Evento clik izquierdo del mouse
                {
                    printf("Hiciste clic izquierdo en (%d, %d) poniendo un dibujo en la posición [%d,%d]\n", x, y, xGrilla, yGrilla);
                    dibujar(ventana, renderer,
                         mapa[yGrilla][xGrilla] == 0 ? square2 :
                         mapa[yGrilla][xGrilla] == 1 ? one :
                         mapa[yGrilla][xGrilla] == 2 ? two :
                         mapa[yGrilla][xGrilla] == 3 ? three :
                         mapa[yGrilla][xGrilla] == -1 ? mine : flag, xGrilla, yGrilla);
                }
                else if (boton == SDL_BUTTON_RIGHT) //Evento click derecho del mouse
                {
                    printf("Hiciste clic derecho en (%d, %d) colocando bandera\n", x, y);
                    // borrarPantalla(ventana, renderer);
                    dibujar(ventana, renderer, flag, xGrilla, yGrilla);
                }
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }

    SDL_DestroyRenderer(renderer); //Funcion para destruir el renderer
    SDL_DestroyWindow(ventana); //Funcion para destrir la ventana
    SDL_Quit(); //Funcion para parar SDL init
    return 0;
}
