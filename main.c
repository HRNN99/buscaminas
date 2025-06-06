#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int renderizarTexto(TTF_Font *font, const char *texto, SDL_Color *color, SDL_Renderer *render, int x, int y);
int leerConfiguracion(int*, int*,int*, char*);

int main(int argc, char *argv[])
{
    int filas = 0, columnas = 0, minasEnMapa = 0;
    char rutaFuente[100];

    //Creacion archivo log
    
    FILE* archivoLog = fopen("partida.log", "w + t");
    Log log;
    setLog(&log, NULL, -1, -1, "Inicio del juego");
    escribirArchivoLog(archivoLog, &log);

    // Lectura del archivo de configuarcion
    leerConfiguracion(&filas, &columnas, &minasEnMapa, rutaFuente);

    Casilla **mapa = matrizCrear(filas, columnas, sizeof(Casilla)); // Creacion de la matriz
    Juego juego;
    juego.cantCasillasPresionadas = 0;
    juego.mapa = mapa;
    mapaVacio(juego.mapa, filas, columnas);                          // Se establecen los valores defecto de la matriz

    int minasCord[minasEnMapa][2]; // Guardado de posicion de las minas en una matriz aparte
    mapaLlenar(juego.mapa, filas, columnas, minasEnMapa, minasCord); //

    // Iniciar SDL con funcion Video
    SDL_Init(SDL_INIT_VIDEO);

    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(rutaFuente, 24);
    if (!font)
    {
        printf("Error cargando fuente: %s\n", TTF_GetError());
        return ERROR_FUENTE;
    }
    char nombreJugador[100];

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

    fondoColor(renderer); //Funcion para establecer fondo del render color defecto
    casillaColocacion(renderer, filas, columnas); //Funcion para colocar todas las casillas visuales

    int boton, xGrilla, yGrilla, renderizarGanado = 0, puntajePartida = 0;
    while (corriendo)
    { // While para mantener el programa corriendo

        SDL_RenderPresent(renderer);
        SDL_Window *ventanaGanado;
        SDL_Renderer *rendererGanado;

        if (renderizarGanado)
        {
            //Inicio la lectura de teclado
            SDL_StartTextInput();
            // Limpia pantalla
            SDL_SetRenderDrawColor(rendererGanado, 0, 0, 0, 255); // negro
            SDL_RenderClear(rendererGanado);
            // Renderizar "Puntaje" y "Nombre:"
            SDL_Color blanco = {255, 255, 255, 255};
            char textoPuntaje[21] = "Puntaje: ";
            char puntajeChar[12];
            strcat(textoPuntaje, itoa(puntajePartida, puntajeChar, 10)); //Armado de String a imprimir
            renderizarTexto(font, textoPuntaje, &blanco, rendererGanado, 50, 50);
            renderizarTexto(font, "Ingrese su nombre:", &blanco, rendererGanado, 50, 100);
            renderizarTexto(font, nombreJugador, &blanco, rendererGanado, 50, 120);

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

                    escribirArchivoLog(archivoLog, &log);
                    casillaEstado(renderer, ventana, &juego, minasCord, minasEnMapa , filas , columnas , xGrilla , yGrilla);
                    if (juego.cantCasillasPresionadas == (filas * columnas) - minasEnMapa)
                    {
                        puts("Ganaste el juego!");
                        renderizarGanado = 1;
                        *nombreJugador = '\0'; // Limpieza por si se presionaron teclas al jugar
                        // Funcion para crear ventana con posicion especifica, dimension y banderas.
                        ventanaGanado = SDL_CreateWindow("Ganaste!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX - 100, TAMY - 100, 2);
                        // Funcion para crear el renderizado en ventana acelerado por hardware
                        rendererGanado = SDL_CreateRenderer(ventanaGanado, -1, SDL_RENDERER_ACCELERATED);
                        // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
                        SDL_SetRenderDrawBlendMode(rendererGanado, SDL_BLENDMODE_BLEND);
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
                // Actualizacion de nombreJugador al presionar una tecla
                if (strlen(nombreJugador) + strlen(e.text.text) < 100)
                {
                    strcat(nombreJugador, e.text.text);
                    renderizarGanado = 1;
                }
                break;
            case SDL_KEYDOWN:
                // Borrado de letra al presionar borrar
                if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(nombreJugador) > 0)
                {
                    nombreJugador[strlen(nombreJugador) - 1] = '\0';
                    renderizarGanado = 1;
                }
                // Guardado de puntaje al presionar Enter
               if (e.key.keysym.sym == SDLK_RETURN && strlen(nombreJugador) > 0)
               {
                    SDL_StopTextInput(); //Cierro la lectura de teclado
                    FILE* aPuntuacion = fopen("puntuacion.txt", "a");
                    if(!aPuntuacion)
                    {
                        puts("Error al abrir el archivo puntuacion.txt");
                        return ERROR_ARCHIVO;
                    }
                    Jugador jugador;
                    strcpy(jugador.nombre, nombreJugador);
                    jugador.puntaje = puntajePartida;
                    fprintf(aPuntuacion, "%05d | %s\n", jugador.puntaje, jugador.nombre);
                    fclose(aPuntuacion);
                    renderizarGanado = 0;
                    FinalizarVentanaSDL(ventanaGanado, rendererGanado); // Funcion para la finalizacion de SDL y sus componentes
                    fclose(archivoLog);
               }
                break;
            }
        }

        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }
    return EJECUCION_OK;
}

int renderizarTexto(TTF_Font *font, const char *texto, SDL_Color *color, SDL_Renderer *render, int x, int y)
{
    if(!strlen(texto) > 0){ //Evita el renderizado con cero caracteres
        return 1;
    }
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, texto, *color);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(render, textSurface);
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(render, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

int leerConfiguracion(int* filas, int* columnas, int* minasEnMapa, char* rutaFuente){
    FILE* config = fopen("buscaminas.conf", "r+t");
    if (!config)
    {
        puts("Error al abrir archivo de configuracion. Cerrando juego...");
        return ERROR_ARCHIVO;
    }

    if (fscanf(config, "DIMENSION_MAPA = %d\n", filas) != 1) {
        puts("Error al leer DIMENSION_MAPA.");
        fclose(config);
        return ERROR_ARCHIVO;
    }
    if(*filas < 8 || *filas > 32){
        puts("Error de configuracion DIMENSION_MAPA valores validos entre 8 y 32.");
        fclose(config);
        return ERROR_CONFIGURACION;
    }
    *columnas = *filas; // Mapa cuadrado siempre

    char minasTexto[5];
    if (fscanf(config, "CANTIDAD_MINAS = %s\n", minasTexto) != 1) {
        puts("Error al leer CANTIDAD_MINAS.");
        fclose(config);
        return ERROR_ARCHIVO;
    }
    char* porcentaje = strchr(minasTexto, '%');
    *minasEnMapa = atoi(minasTexto);

    // Lo convierto a un porcentual del mapa
    if (porcentaje) 
        *minasEnMapa = round(((*filas)*(*columnas))*((float)*minasEnMapa/100));

    if (fscanf(config, "RUTA_FUENTE = %s\n", rutaFuente) != 1) {
        puts("Error al leer RUTA_FUENTE.");
        fclose(config);
        return ERROR_ARCHIVO;
    }
    printf("%d, %d, %s", *filas, *minasEnMapa, rutaFuente);
    fclose(config);
    
}

int escribirArchivoLog(FILE* archivoLog, Log* log)
{
    if(log->coordXY[0] == -1 && log->coordXY[1] == -1)
    {
          fprintf(archivoLog, "%s|%s \n", log->fechaHora, log->tipoEvento);
    }else
    {
        fprintf(archivoLog, "%s|%s|coord x = %d|coord y = %d \n", log->fechaHora, log->tipoEvento, log->coordXY[0],log->coordXY[1]);
    }
}
    