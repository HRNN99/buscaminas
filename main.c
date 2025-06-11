#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int leerConfiguracion(int*, int*,int*, char*);
int escribirArchivoLog(FILE* archivoLog, Log* log);

int main(int argc, char *argv[])
{

    int filas = 0, columnas = 0, minasEnMapa = 0;
    char rutaFuente[100];

    //Creacion archivo log

    FILE* archivoLog = fopen("partida.log", "w");
    if(!archivoLog)
    {
        puts("Error creando el archivo log");
        return ERROR_ARCHIVO;
    }
    Log log;
    setLog(&log, -1, -1, "Inicio del juego");
    escribirArchivoLog(archivoLog, &log);

    // Lectura del archivo de configuarcion
    leerConfiguracion(&filas, &columnas, &minasEnMapa, rutaFuente);

    // Iniciar SDL con funcion Video
    SDL_Init(SDL_INIT_VIDEO);

    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(rutaFuente, 64);
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

    MenuItem menu[] = {
        {"Nueva Partida", {100,200,200,50}},
        {"Cargar Partida", {100,270,200,50}},
        {"Salir",          {100,340,200,50}}
    };

    int menu_count = sizeof(menu) / sizeof(menu[0]);
    int seleccion = 0;

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

    Juego juego;
    juego.mapa = mapa;

    //Iniciacion de valores de mapa
    mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);

    putchar('\n');

    //Imprimir mapa
    mapaImprimir(juego.mapa , filas , columnas);

    //////////////////////////////////////////////////////////////////////

    SDL_Event e; // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    int boton, xGrilla, yGrilla, renderizarGanado = 0, fontSize = 16, casillasLibresDeMinas = (filas * columnas) - minasEnMapa;
    time_t current_time;

    //Variable para estados
    EstadoJuego estado_actual = ESTADO_MENU;

    // While para mantener el programa corriendo
    while (corriendo){

        SDL_Window *ventanaGanado;
        SDL_Renderer *rendererGanado;
        if(1){
            int G=2;
            int pad = G*4;

            int anchoM = filas * PIXELES_X_LADO + 4;
            int altoC = 28;
            int anchoI = anchoM + 16;
            int altoI = pad + altoC + pad + anchoM + pad;

            //renderizarTexto(font, fontSize, "Minas:", GF, GS,renderer, (pad*3)+anchoM+22, pad+(altoC/2));
            char bombasEnMapaTexto[21] = "";
            itoa(juego.cantMinasEnInterfaz, bombasEnMapaTexto, 10); //Armado de String a imprimir
            renderizarTexto(font, 46, bombasEnMapaTexto, RR, NN, renderer, picords.x+10 , rbutton.y+12);

            //renderizarTexto(font, fontSize, "Puntaje:", GF, GS, renderer, pad*3, pad+(altoC/2));
            char puntaje[21] = "";
            itoa(juego.puntaje, puntaje, 10); //Armado de String a imprimir
            renderizarTexto(font, fontSize, puntaje, GF, GS, renderer, (pad*3)+anchoM+22, pad+(altoC/2)+fontSize+2);

            // Aumento de puntaje por segundo
            if (!juego.finPartida){
                current_time = time(NULL);
                juego.puntaje = difftime(current_time, juego.start_time);
            }
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
            strcat(textoPuntaje, itoa(juego.puntaje, puntajeChar, 10)); //Armado de String a imprimir
            renderizarTexto(font, 24, textoPuntaje, BB, GS,rendererGanado, 50, 50);
            renderizarTexto(font, 24, "Ingrese su nombre:", BB, GS, rendererGanado, 50, 100);
            renderizarTexto(font, 24, nombreJugador, BB, GS,rendererGanado, 50, 120);

            // Mostrar todo
            SDL_RenderPresent(rendererGanado);
            renderizarGanado = 0;
            juego.finPartida = true;
        }

        while (SDL_PollEvent(&e))
        { // Registrando eventos

            switch (e.type)
            {
            case SDL_QUIT:
                setLog(&log, -1, -1, "Salida de SDL");
                escribirArchivoLog(archivoLog, &log);
                printf("Saliendo de SDL\n");
                corriendo = 0;
                matrizDestruir(juego.mapa, filas);             // Se libera la memoria de la matriz mapa
                FinalizarSDL(ventana, renderer, font, EXIT_SUCCESS, archivoLog); // Funcion para la finalizacion de SDL y sus componentes
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    setLog(&log, -1, -1, "Salida de pantalla de ganado");
                    escribirArchivoLog(archivoLog, &log);
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
                    // Click en boton de reinicio
                    if((rbutton.x * TAM_PIXEL <= e.button.x && e.button.x <= (rbutton.x + 28) * TAM_PIXEL)
                        &&(rbutton.y * TAM_PIXEL <= e.button.y && e.button.y <= (rbutton.y + 28) * TAM_PIXEL)){

                        interfaz(renderer,&picords,filas,&rbutton);
                        mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);
                        system("cls");

                        printf("Reiniciaste el mapa \n\n");

                        //Imprimir mapa
                        mapaImprimir(juego.mapa , filas , columnas);
                    }
                    else if(!juego.finPartida){
                        printf("Hiciste clic izquierdo en la casilla (%i,%i)\n", e.button.x, e.button.y);
                        setLog(&log, xGrilla, yGrilla, "click izquierdo");
                        escribirArchivoLog(archivoLog, &log);
                        casillaEstado(renderer, ventana, &juego, &minasCoord, minasEnMapa, xGrilla , yGrilla , &picords);

                        if (juego.cantCasillasPresionadas == casillasLibresDeMinas)
                        {
                            setLog(&log, -1, -1, "Juego ganado");
                            escribirArchivoLog(archivoLog, &log);
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
                else if (boton == SDL_BUTTON_RIGHT && !juego.finPartida && !juego.mapa[yGrilla][xGrilla].presionada)
                { // Evento click derecho del mouse
                    printf("Hiciste clic derecho en la casilla (%i, %i) colocando bandera\n", xGrilla, yGrilla);
                    setLog(&log, xGrilla, yGrilla, "click derecho");
                    escribirArchivoLog(archivoLog, &log);
                    casillaBandera(renderer, &juego, xGrilla , yGrilla , &picords, &juego.cantMinasEnInterfaz);
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

                switch (e.key.keysym.sym){

                    case SDLK_UP:   seleccion = (seleccion - 1 + menu_count) % menu_count; break;
                    case SDLK_DOWN: seleccion = (seleccion + 1) % menu_count; break;
                    case SDLK_RETURN:
                        switch(seleccion){
                            case 0:
                                interfaz(renderer,&picords,filas,&rbutton);
                                mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);
                                //system("cls");
                                SDL_RenderPresent(renderer);
                                break;
                            case 1:
                                break;
                            case 2:
                                //Salir
                                corriendo = false;
                                printf("Saliendo...\n");
                                break;
                        }
                }

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
                        setLog(&log, -1, -1, "Error al abrir el archivo de puntuacion.");
                        escribirArchivoLog(archivoLog, &log);
                        puts("Error al abrir el archivo puntuacion.txt");
                        fclose(archivoLog);
                        return ERROR_ARCHIVO;
                    }
                    fprintf(aPuntuacion, "%05d | %s\n", juego.puntaje, nombreJugador);
                    fclose(aPuntuacion);
                    renderizarGanado = 0;
                    FinalizarVentanaSDL(ventanaGanado, rendererGanado); // Funcion para la finalizacion de SDL y sus componentes

               }
                break;
            }
        }

        //////////////////////////////////////////////////////////////////////

        SDL_SetRenderDrawColor(renderer , 0 , 0 , 0 , 255);
        SDL_RenderClear(renderer);

        int win_width, win_height;
        SDL_GetWindowSize(ventana , &win_width , &win_height);

        int base_y = 50;
        int espacio = 50;

        for(int i = 0 ; i < menu_count ; i++){

            int text_width, text_height;
            TTF_SizeText(font , menu[i].texto , &text_width , &text_height);

            menu[i].rect.x = (win_width - text_width) / 2;
            menu[i].rect.y = base_y + i*espacio;

            //Establecemos un color para las letras del menu
            SDL_Color colorTexto = {255,255,255,255};
            //Se establece un color de fondo, segun si el item esta seleccionado o no
            SDL_Color colorFondo = (i == seleccion) ? (SDL_Color){255,100,255,255} : (SDL_Color){100,100,100,255};

            //Se crea una superficie de texto con el texto contenido en el menu[i]
            SDL_Surface *surface = TTF_RenderText_Solid(font , menu[i].texto , colorTexto);
            //Convierte la superficie en una textura para poder ser renderizada
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer , surface);

            //Se establece el color de dibujado usando color de fondo
            SDL_SetRenderDrawColor(renderer , colorFondo.r , colorFondo.g , colorFondo.b , colorFondo.a);
            //Se dibuja el rectangulo lleno en el render
            SDL_RenderFillRect(renderer , &menu[i].rect);

            //Calcula automaticamente el ancho y alto de texto renderizado, y actualiza valores de menu.rect
            SDL_QueryTexture(texture , NULL , NULL ,&menu[i].rect.w , &menu[i].rect.h);
            //Se dibuja el texto en el render
            SDL_RenderCopy(renderer , texture , NULL , &menu[i].rect);

            //Se liberan variables temporales
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

        }

        //////////////////////////////////////////////////////////////////////

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // (60 fps) Esta pausa es para evitar que el procesador se ponga al 100% renderizando constantemente.
    }
    fclose(archivoLog);
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
    //printf("%d, %d, %s", *filas, *minasEnMapa, rutaFuente);
    fclose(config);

    return 0;
}

int escribirArchivoLog(FILE* archivoLog, Log* log)
{
    if(log->coordXY[0] == -1 && log->coordXY[1] == -1)
    {
        fprintf(archivoLog, "[%d-%d-%d %02d:%02d:%02d] %-15s\n",
            log->fechaHora.tm_year + 1900, log->fechaHora.tm_mon + 1, log->fechaHora.tm_mday,
            log->fechaHora.tm_hour, log->fechaHora.tm_min, log->fechaHora.tm_sec, log->tipoEvento);
    }else
    {
        fprintf(archivoLog, "[%d-%d-%d %02d:%02d:%02d] %-15s | coordenadas: (%d , %d)\n",
            log->fechaHora.tm_year + 1900, log->fechaHora.tm_mon + 1, log->fechaHora.tm_mday,
            log->fechaHora.tm_hour, log->fechaHora.tm_min, log->fechaHora.tm_sec, log->tipoEvento,
            log->coordXY[0], log->coordXY[1]);
    }
    return 0;
}
