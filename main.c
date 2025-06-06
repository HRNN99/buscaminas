#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int leerConfiguracion(int*, int*,int*, char*);

int main(int argc, char *argv[])
{
    int filas = 0, columnas = 0, minasEnMapa = 0;
    char rutaFuente[100];

    // Lectura del archivo de configuarcion
    leerConfiguracion(&filas, &columnas, &minasEnMapa, rutaFuente);

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
    int TAMX =  TAM_PIXEL * (columnas * PIXELES_X_LADO + 20);
    int TAMY =  TAM_PIXEL * (filas * PIXELES_X_LADO + 4 + 3*8 + 28);
    // Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //////////////////////////////////////////////////////////////////////

    fondoColor(renderer); //Funcion para establecer fondo del render color defecto

    Coord picords = {0,0};
    Coord rbutton = {0,0};
    interfaz(renderer , &picords , filas , &rbutton); //Funcion para colocar la interfaz

    //////////////////////////////////////////////////////////////////////

    //Creacion de matriz mapa
    Casilla **mapa = matrizCrear(filas, columnas, sizeof(Casilla));

    //Vector de coordenadas para las minas
    Coord minasCoord[minasEnMapa];

    //Iniciacion de valores de mapa
    mapaReiniciar(renderer , &picords , mapa , filas , columnas , &minasCoord , minasEnMapa);

    Juego juego;
    juego.cantCasillasPresionadas = 0;
    juego.mapa = mapa;

    putchar('\n');

    // TEST ONLY //Impresion de matriz
    for (size_t i = 0; i < filas; i++){

        for (size_t j = 0; j < columnas; j++){

            printf("%3d ", juego.mapa[i][j].estado);
        }
        printf("\n\n");
    }

    //////////////////////////////////////////////////////////////////////

    SDL_Event e; // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    int boton, xGrilla, yGrilla, renderizarGanado = 0, puntajePartida = 0;
    // time_t start_time = time(NULL); // Get the starting time
    // time_t current_time;
    // current_time = time(NULL);
    // puntajePartida = difftime(current_time, start_time);
    // While para mantener el programa corriendo
    while (corriendo){
        SDL_RenderPresent(renderer);
        SDL_Window *ventanaGanado;
        SDL_Renderer *rendererGanado;
        if(1){
            int G=2;
            int pad = G*4;

            int anchoM = filas * PIXELES_X_LADO + 4;
            int altoC = 28;
            int anchoI = anchoM + 16;
            int altoI = pad + altoC + pad + anchoM + pad;

            renderizarTexto(font, 16, "Puntaje:", GF, renderer, pad+14, pad+14);
            renderizarTexto(font, 16, "Bombas:", GF, renderer, (pad*2)+anchoM+28, pad+14);
        }
        if (renderizarGanado)
        {
            //Inicio la lectura de teclado
            SDL_StartTextInput();
            // Limpia pantalla
            SDL_SetRenderDrawColor(rendererGanado, 0, 0, 0, 255); // negro
            SDL_RenderClear(rendererGanado);
            // Renderizar "Puntaje" y "Nombre:"
            char textoPuntaje[21] = "Puntaje: ";
            char puntajeChar[12];
            strcat(textoPuntaje, itoa(puntajePartida, puntajeChar, 10)); //Armado de String a imprimir
            renderizarTexto(font, 24, textoPuntaje, BB, rendererGanado, 50, 50);
            renderizarTexto(font, 24, "Ingrese su nombre:", BB, rendererGanado, 50, 100);
            renderizarTexto(font, 24, nombreJugador, BB, rendererGanado, 50, 120);

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
                FinalizarSDL(ventana, renderer, font, EXIT_SUCCESS); // Funcion para la finalizacion de SDL y sus componentes
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
                xGrilla = (e.button.x - (picords.x*TAM_PIXEL)) / ( PIXELES_X_LADO * TAM_PIXEL);
                yGrilla = (e.button.y - (picords.y*TAM_PIXEL)) / ( PIXELES_X_LADO * TAM_PIXEL);

                if (boton == SDL_BUTTON_LEFT)
                { // Evento clik izquierdo del mouse

                    if((rbutton.x * TAM_PIXEL <= e.button.x && e.button.x <= (rbutton.x + 28) * TAM_PIXEL)
                        &&(rbutton.y * TAM_PIXEL <= e.button.y && e.button.y <= (rbutton.y + 28) * TAM_PIXEL)){

                        printf("Reiniciaste mapa \n");
                        mapaReiniciar(renderer , &picords , mapa , filas , columnas , &minasCoord , minasEnMapa);
                    }

                    else{
                    printf("Hiciste clic izquierdo en la casilla (%i,%i)\n", e.button.x, e.button.y);
                    casillaEstado(renderer, ventana, &juego, &minasCoord, minasEnMapa , filas , columnas , xGrilla , yGrilla , &picords);
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
                }

                else if (boton == SDL_BUTTON_RIGHT)
                { // Evento click derecho del mouse
                    printf("Hiciste clic derecho en la casilla (%i, %i) colocando bandera\n", xGrilla, yGrilla);
                    casillaBandera(renderer, xGrilla, yGrilla , &picords);
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
               }
                break;
            }
        }

        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }
    return EJECUCION_OK;
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

    return 0;
}

