#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int main(int argc, char *argv[])
{

    //    char *text;
    //    char *composition;
    //    Sint32 cursor;
    //    Sint32 selection_len;

    int filas = 10, columnas = 10, minasEnMapa = 2;
    //    printf("ingrese la cantidad de filas: "); scanf("%d", &filas);
    //    printf("ingrese la cantidad de columnas: "); scanf("%d", &columnas);
    //    printf("ingrese la cantidad de minas: "); scanf("%d", &minasEnMapa);

    int minasCord[minasEnMapa][2];

    Casilla **mapa = matrizCrear(filas, columnas, sizeof(Casilla)); // Creacion de la matriz
    Juego juego;
    juego.cantCasillasPresionadas = 0;
    juego.mapa = mapa;
    mapaVacio(juego.mapa, filas, columnas);                          // Se establecen los valores defecto de la matriz
    mapaLlenar(juego.mapa, filas, columnas, minasEnMapa, minasCord); //

    // Iniciar SDL con funcion Video
    SDL_Init(SDL_INIT_VIDEO);
    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (!font)
    {
        printf("Error cargando fuente: %s\n", TTF_GetError());
        return 1;
    }
    char inputText[100] = " "; // Tiene que estar con un espacio porque si no rompe al renderizar una cadena vacia
    SDL_StartTextInput();
    char nombreVentana[100];
    // String formateado para el titulo de ventana
    sprintf(nombreVentana, "Buscaminas %ix%i", filas, columnas);
    // Tamaño de ancho y altura de la ventana, utilizo 1 sola variable ya que sera cuadrada
    int TAMX = columnas * (TAM_PIXEL * PIXELES_X_LADO + PX_PADDING);
    int TAMY = filas * (TAM_PIXEL * PIXELES_X_LADO + PX_PADDING);
    // Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    putchar('\n');

    // TEST ONLY //Impresion de matriz
    for (size_t i = 0; i < filas; i++)
    {

        for (size_t j = 0; j < columnas; j++)
        {

            printf("%3d ", juego.mapa[i][j].estado);
        }
        printf("\n\n");
    }

    SDL_Event e;       // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    fondoColor(renderer);        // Funcion para establecer fondo del render color defecto
    casillaColocacion(renderer); // Funcion para colocar todas las casillas visuales

    int boton, xGrilla, yGrilla, renderizarGanado = 0;
    while (corriendo)
    { // While para mantener el programa corriendo

        SDL_RenderPresent(renderer);
        SDL_Window *ventanaGanado;
        SDL_Renderer *rendererGanado;

        if (renderizarGanado)
        {

            // Renderizar "Puntaje" y "Nombre:"
            SDL_Color blanco = {255, 255, 255, 255};
            SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Puntaje: 200", blanco);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(rendererGanado, textSurface);
            SDL_Rect textRect = {50, 50, textSurface->w, textSurface->h};
            SDL_RenderCopy(rendererGanado, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            textSurface = TTF_RenderText_Solid(font, "Ingrese su nombre:", blanco);
            textTexture = SDL_CreateTextureFromSurface(rendererGanado, textSurface);
            SDL_Rect textRect2 = {50, 100, textSurface->w, textSurface->h};
            SDL_RenderCopy(rendererGanado, textTexture, NULL, &textRect2);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            // Renderizar entrada del usuario
            SDL_Surface *inputSurface = TTF_RenderText_Solid(font, inputText, blanco);
            SDL_Texture *inputTexture = SDL_CreateTextureFromSurface(rendererGanado, inputSurface);
            SDL_Rect inputRect = {50, 60, inputSurface->w, inputSurface->h};
            SDL_RenderCopy(rendererGanado, inputTexture, NULL, &inputRect);
            SDL_FreeSurface(inputSurface);
            SDL_DestroyTexture(inputTexture);

            // Mostrar todo
            SDL_RenderPresent(rendererGanado);
            renderizarGanado = 0;
        }
        if (SDL_PollEvent(&e))
        { // Registrando eventos

            switch (e.type)
            {
            case SDL_QUIT:
                printf("Saliendo de SDL\n");
                corriendo = 0;
                matrizDestruir(juego.mapa, filas);             // Se libera la memoria de la matriz mapa
                FinalizarSDL(ventana, renderer, EXIT_SUCCESS); // Funcion para la finalizacion de SDL y sus componentes
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    printf("Saliendo de pantalla de ganado\n");
                    renderizarGanado = 0;
                    FinalizarVentanaSDL(ventanaGanado, rendererGanado); // Funcion para la finalizacion de SDL y sus componentes
                }
                break;
            case SDL_MOUSEBUTTONDOWN:

                boton = e.button.button; // Alias del boton presionado

                // Se dividen los pixeles para obtener un valor de grilla
                xGrilla = e.button.x / (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);
                yGrilla = e.button.y / (PIXELES_X_LADO * TAM_PIXEL + PX_PADDING);

                if (boton == SDL_BUTTON_LEFT)
                { // Evento clik izquierdo del mouse
                    printf("Hiciste clic izquierdo en la casilla (%i,%i)\n", xGrilla, yGrilla);
                    casillaEstado(renderer, &juego, filas, columnas, xGrilla, yGrilla);
                    if (juego.cantCasillasPresionadas == (filas * columnas) - minasEnMapa)
                    // if (1)
                    {
                        puts("Ganaste el juego!");
                        renderizarGanado = 1;
                        // Funcion para crear ventana con posicion especifica, dimension y banderas.
                        ventanaGanado = SDL_CreateWindow("Ganaste!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX - 100, TAMY - 100, 2);
                        // Funcion para crear el renderizado en ventana acelerado por hardware
                        rendererGanado = SDL_CreateRenderer(ventanaGanado, -1, SDL_RENDERER_ACCELERATED);
                        // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
                        SDL_SetRenderDrawBlendMode(rendererGanado, SDL_BLENDMODE_BLEND);

                        // Limpia pantalla
                        SDL_SetRenderDrawColor(rendererGanado, 0, 0, 0, 255); // negro
                        SDL_RenderClear(rendererGanado);
                    }
                }

                else if (boton == SDL_BUTTON_RIGHT)
                { // Evento click derecho del mouse
                    printf("Hiciste clic derecho en la casilla (%i, %i) colocando bandera\n", xGrilla, yGrilla);
                    casillaBandera(renderer, xGrilla, yGrilla);
                }
                printf("Presionadas: %d\n", juego.cantCasillasPresionadas);
                break;
            case SDL_TEXTINPUT:
                if (strlen(inputText) + strlen(e.text.text) < 100)
                {
                    strcat(inputText, e.text.text);
                    renderizarGanado = 1;
                }
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0)
                {
                    inputText[strlen(inputText) - 1] = '\0';
                    renderizarGanado = 1;
                }
                break;
            }
        }

        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }
    return 0;
}
