#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

bool linea_ignorable(const char* linea);
int cargar_dificultades(const char* archivo , Dificultad* difs , int num_dif);
int escribirArchivoLog(FILE* archivoLog, Log* log);

int main(int argc, char *argv[]){

    int filas = 16, columnas = 16, minasEnMapa = 0;
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
    //leerConfiguracion(&filas, &columnas, &minasEnMapa, rutaFuente);

    // Iniciar SDL con funcion Video
    SDL_Init(SDL_INIT_VIDEO);

    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("Fonts/digital-7.ttf", 32);
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

    const char* menu_items[] = {
        "Nueva Partida",
        "Cargar Partida",
        "Salir"
    };
    int menu_count = sizeof(menu_items) / sizeof(menu_items[0]);
    int seleccion_menu = 0;

    const char* dificultad_items[] = {
        "Facil",
        "Intermedio",
        "Dificil"
    };
    int dificultad_count = sizeof(dificultad_items) / sizeof(dificultad_items[0]);
    int seleccion_dificultad = 0;

    Dificultad dificultades[dificultad_count];

    cargar_dificultades("buscaminas.conf" , dificultades , dificultad_count);

    //////////////////////////////////////////////////////////////////////

    Coord picords = {0,0};
    Coord rbutton = {0,0};

    //////////////////////////////////////////////////////////////////////

    //Creacion de matriz mapa
    //Casilla **mapa = matrizCrear(filas, columnas, sizeof(Casilla));

    //Vector de coordenadas para las minas
    Coord minasCoord[minasEnMapa];

    Juego juego;
    juego.mapa = matrizCrear(1 , 1 , sizeof(Casilla));
    juego.iniciado = false;

    //////////////////////////////////////////////////////////////////////

    SDL_Event e; // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    int boton , xGrilla , yGrilla , renderizarGanado = 0 , fontSize = 16 , casillasLibresDeMinas = (filas * columnas) - minasEnMapa;
    time_t current_time;

    //Variable para estados
    EstadoJuego estado_actual = ESTADO_MENU;

    // While para mantener el programa corriendo
    while (corriendo){

        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                corriendo = false;
                matrizDestruir(juego.mapa , juego.dificultad.dimension);
                printf("\n---Cerrando Ventana---\n");
            }

            switch(estado_actual){
                case ESTADO_MENU:
                    manejar_eventos_menu(&e , &estado_actual , &seleccion_menu , menu_count);
                    break;

                case ESTADO_DIFICULTAD:
                    manejar_eventos_dificultad(&e , &estado_actual , &seleccion_dificultad , dificultad_count , &juego , dificultades , ventana);
                    break;

                case ESTADO_JUGANDO:
                    manejar_eventos_juego(&e , &estado_actual , &juego , &minasCoord , minasEnMapa , &picords , &rbutton);
                    break;

                case ESTADO_SALIENDO:
                    corriendo = false;
                    matrizDestruir(juego.mapa , juego.dificultad.dimension);
                    printf("\nSaliendo...\n");
                    break;
            }
        }

        //SDL_RenderClear(renderer);

        switch(estado_actual){
            case ESTADO_MENU:

                dibujar_menu(renderer , ventana , font , menu_items , menu_count , &seleccion_menu);
                break;

            case ESTADO_DIFICULTAD:

                dibujar_menu(renderer , ventana , font , dificultad_items , dificultad_count , &seleccion_dificultad);
                break;

            case ESTADO_JUGANDO:

                interfaz(renderer , &picords , juego.dificultad.dimension ,&rbutton);

                if(!juego.iniciado){

                    mapaReiniciar(renderer , &picords , &juego , juego.dificultad.dimension , &minasCoord , juego.dificultad.cantidad_minas);
                    system("cls");
                    mapaImprimir(juego.mapa , juego.dificultad.dimension , juego.dificultad.dimension);

                }

                casillaColocacion(juego.mapa, renderer, juego.dificultad.dimension , juego.dificultad.dimension , &picords);
                break;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);

        /*
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
                             // Se libera la memoria de la matriz mapa
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
        */
    }
    matrizDestruir(juego.mapa , juego.dificultad.dimension);
    fclose(archivoLog);
    return EJECUCION_OK;
}

void manejar_eventos_menu(SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int items_count){

    switch(e->type){

        case SDL_KEYDOWN:

            switch(e->key.keysym.sym){

                case SDLK_UP:
                    *seleccion = (*seleccion - 1 + items_count) % items_count;
                    break;

                case SDLK_DOWN:
                    *seleccion = (*seleccion + 1) % items_count;
                    break;

                case SDLK_RETURN:
                    switch(*seleccion){

                        case 0:
                            *estado_actual = ESTADO_DIFICULTAD;
                            break;

                        case 1:
                            *estado_actual = ESTADO_CARGAR;
                            break;

                        case 2:
                            *estado_actual = ESTADO_SALIENDO;
                            break;
                    }
                    break;
            }
            break;
    }
}

void manejar_eventos_dificultad(SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int items_count , Juego* juego , Dificultad* difs , SDL_Window* ventana){

    switch(e->type){

        case SDL_KEYDOWN:

            switch(e->key.keysym.sym){

                case SDLK_UP:
                    *seleccion = (*seleccion - 1 + items_count) % items_count;
                    break;

                case SDLK_DOWN:
                    *seleccion = (*seleccion + 1) % items_count;
                    break;

                case SDLK_RETURN:
                    switch(*seleccion){

                        case 0:
                            //juego->dificultad.nombre = difs[0].nombre;
                            juego->dificultad.dimension = difs[0].dimension;
                            juego->dificultad.cantidad_minas = difs[0].cantidad_minas;

                            juego->mapa = matrizCrear(juego->dificultad.dimension , juego->dificultad.dimension , sizeof(Casilla)); //Mover a otro lado despues
                            *estado_actual = ESTADO_JUGANDO;
                            SDL_SetWindowSize(ventana , TAM_PIXEL * (difs[0].dimension * PIXELES_X_LADO + 20) , TAM_PIXEL * (difs[0].dimension * PIXELES_X_LADO + 4 + 3*8 + 28));
                            break;

                        case 1:
                            //juego->dificultad.nombre = difs[1].nombre;
                            juego->dificultad.dimension = difs[1].dimension;
                            juego->dificultad.cantidad_minas = difs[1].cantidad_minas;

                            juego->mapa = matrizCrear(juego->dificultad.dimension , juego->dificultad.dimension , sizeof(Casilla));
                            *estado_actual = ESTADO_JUGANDO;
                            SDL_SetWindowSize(ventana , TAM_PIXEL * (difs[1].dimension * PIXELES_X_LADO + 20) , TAM_PIXEL * (difs[1].dimension * PIXELES_X_LADO + 4 + 3*8 + 28));
                            break;

                        case 2:
                            //juego->dificultad.nombre = difs[2].nombre;
                            juego->dificultad.dimension = difs[2].dimension;
                            juego->dificultad.cantidad_minas = difs[2].cantidad_minas;

                            juego->mapa = matrizCrear(juego->dificultad.dimension , juego->dificultad.dimension , sizeof(Casilla));
                            *estado_actual = ESTADO_JUGANDO;
                            SDL_SetWindowSize(ventana , TAM_PIXEL * (difs[2].dimension * PIXELES_X_LADO + 20) , TAM_PIXEL * (difs[2].dimension * PIXELES_X_LADO + 4 + 3*8 + 28));
                            break;
                    }
                    break;
            }
            break;
    }
}

void manejar_eventos_juego(SDL_Event *e , EstadoJuego *estado_actual , Juego* juego , Coord* minasCoord , int minas , Coord* picords , Coord* rbutton){

    int xG = ((e->button.x - (picords->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int yG = ((e->button.y - (picords->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));

    switch(e->type){

        case SDL_MOUSEBUTTONDOWN:

            switch(e->button.button){

                case SDL_BUTTON_LEFT:

                    if((rbutton->x * TAM_PIXEL <= e->button.x && e->button.x <= (rbutton->x + 28) * TAM_PIXEL) &&
                       (rbutton->y * TAM_PIXEL <= e->button.y && e->button.y <= (rbutton->y + 28) * TAM_PIXEL))

                        juego->iniciado = false;

                    printf("Hiciste click en la casilla (%i , %i)\n",xG,yG);
                    casillaEstado(juego , minasCoord , minas , xG , yG);
                    break;

                case SDL_BUTTON_RIGHT:
                    printf("Pusiste una bandera en la casilla (%i , %i)\n",xG,yG);
                    casillaBandera(juego , xG , yG);
                    break;
            }
            break;
    }

}

bool linea_ignorable(const char* linea){
    while(isspace(*linea)) linea++;
    return (*linea == '\0' || *linea == '#');
}

/*
int leerConfiguracion(int* filas, int* columnas, int* minasEnMapa, char* rutaFuente){

    FILE* config = fopen("buscaminas.conf", "r");

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
*/

int cargar_dificultades(const char* archivo , Dificultad* difs , int num_dif){

    FILE* config = fopen(archivo, "r");

    if (!config){
        puts("Error al abrir archivo de configuracion. Cerrando juego...");
        return ERROR_ARCHIVO;
    }

    char linea[150];
    int i = 0;

    char minasTexto[5];

    while(fgets(linea , sizeof(linea) , config) && i <= num_dif){

        if(linea_ignorable(linea)) continue;

        if(strncmp(linea , "DIFICULTAD" , 10) == 0){
            sscanf(linea , "DIFICULTAD = %20[^\n]" , difs[i].nombre);

            printf("%s\n",difs[i].nombre);

            do{ fgets(linea , sizeof(linea) , config); }while(linea_ignorable(linea));

            sscanf(linea , "DIMENSION_MAPA = %i" , &difs[i].dimension);

            if(difs[i].dimension < 8 || difs[i].dimension > 32){
                puts("Error de configuracion DIMENSION_MAPA valores validos entre 8 y 32.");
                fclose(config);
                return ERROR_CONFIGURACION;
            }

            printf("%i\n",difs[i].dimension);

            do{ fgets(linea , sizeof(linea) , config); }while(linea_ignorable(linea));
            if(sscanf(linea , "CANTIDAD_MINAS = %5[^\n]" , minasTexto) == -1){
                puts("Error al leer CANTIDAD_MINAS.");
                fclose(config);
                return ERROR_ARCHIVO;
            }

            char* porcentaje = strchr(minasTexto , '%');
            difs[i].cantidad_minas = atoi(minasTexto);

            if(porcentaje)
                difs[i].cantidad_minas = round(((difs[i].dimension)*(difs[i].dimension))*((float)difs[i].cantidad_minas/100));

            if(difs[i].cantidad_minas < 0){
                puts("Error de configuracion CANTIDAD_MINAS valores negativos no validos");
                fclose(config);
                return ERROR_CONFIGURACION;
            }

            printf("%i\n",difs[i].cantidad_minas);

            i++;
        }

    }

    if(i != num_dif){
        puts("Error al cargar todas las dificultades. Cerrando juego...");
        return ERROR_ARCHIVO;
    }

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
