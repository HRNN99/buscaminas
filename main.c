#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int main(int argc, char *argv[]){

    int filas = 0, columnas = 0, minasEnMapa = 0;
    printf("ingrese la cantidad de filas: "); scanf("%d", &filas);
    printf("ingrese la cantidad de columnas: "); scanf("%d", &columnas);
    printf("ingrese la cantidad de minas: "); scanf("%d", &minasEnMapa);

    int minasCord[minasEnMapa][2];

    Casilla** mapa = (Casilla**)(matrizCrear(filas,columnas,sizeof(Casilla))); //Creacion de la matriz
    mapaVacio(mapa, filas , columnas); //Se establecen los valores defecto de la matriz
    mapaLlenar(mapa , filas , columnas , minasEnMapa , minasCord); //

    //Iniciar SDL con funcion Video
    SDL_Init(SDL_INIT_VIDEO);
    char nombreVentana[100];
    //String formateado para el titulo de ventana
    sprintf(nombreVentana, "Buscaminas %ix%i", filas , columnas);
    //Tamaño de ancho y altura de la ventana, utilizo 1 sola variable ya que sera cuadrada
    int TAMX = columnas * (TAM_PIXEL * PIXELES_X_LADO + PX_PADDING);
    int TAMY = filas * (TAM_PIXEL * PIXELES_X_LADO + PX_PADDING);
    //Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    //Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer* renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    //Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    putchar('\n');

    // TEST ONLY //Impresion de matriz
    for (size_t i = 0; i < filas; i++){

        for (size_t j = 0; j < columnas; j++){

            printf("%3d ", mapa[i][j].estado);
        }
        printf("\n\n");
    }

    SDL_Event e; //Variable para registrar eventos
    int corriendo = 1; //Variable flag true para mantener corriendo el programa

    fondoColor(renderer); //Funcion para establecer fondo del render color defecto
    casillaColocacion(renderer); //Funcion para colocar todas las casillas visuales

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
                    casillaEstado(renderer , mapa , filas , columnas , xGrilla , yGrilla);

                }

                else if (boton == SDL_BUTTON_RIGHT){ //Evento click derecho del mouse
                    printf("Hiciste clic derecho en la casilla (%i, %i) colocando bandera\n", xGrilla , yGrilla);
                    casillaBandera(renderer, xGrilla , yGrilla);
                }
            }
        }

        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }

    matrizDestruir(mapa , filas); //Se libera la memoria de la matriz mapa
    FinalizarSDL(ventana, renderer, EXIT_SUCCESS); //Funcion para la finalizacion de SDL y sus componentes

    return 0;
}
