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

int main(int argc, char *argv[]){

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
    TTF_Font *font = TTF_OpenFont(rutaFuente, 32);
    if (!font)
    {
        printf("Error cargando fuente: %s\n", TTF_GetError());
        return ERROR_FUENTE;
    }

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

    //////////////////////////////////////////////////////////////////////

    //fondoColor(renderer); //Funcion para establecer fondo del render color defecto

    Coord picords = {0,0};
    Coord rbutton = {0,0};
    //interfaz(renderer , &picords , filas , &rbutton); //Funcion para colocar la interfaz

    //////////////////////////////////////////////////////////////////////

    //Creacion de matriz mapa
    Casilla **mapa = matrizCrear(filas, columnas, sizeof(Casilla));

    //Vector de coordenadas para las minas
    Coord minasCoord[minasEnMapa];

    Juego juego;
    juego.mapa = mapa;
    juego.iniciado = false;

    //Iniciacion de valores de mapa
    //mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);

    //Imprimir mapa
    //mapaImprimir(juego.mapa , filas , columnas);

    //////////////////////////////////////////////////////////////////////

    SDL_Event e; // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    int boton, xGrilla, yGrilla, renderizarGanado = 0, fontSize = 16, casillasLibresDeMinas = (filas * columnas) - minasEnMapa;
    time_t current_time;

    //Variable para estados
    EstadoJuego estado_actual = ESTADO_MENU;
    int seleccion = 0;

    // While para mantener el programa corriendo
    while (corriendo){

        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                corriendo = false;
                printf("\n---Cerrando Ventana---\n");
            }

            switch(estado_actual){
                case ESTADO_MENU:
                    manejar_eventos_menu(&e , &estado_actual , &seleccion , menu_count);
                    break;

                case ESTADO_JUGANDO:
                    manejar_eventos_juego(&e , &estado_actual , &juego , &minasCoord , minasEnMapa , &picords , &rbutton);
                    break;

                 case ESTADO_GANADO:

                 if(e.type == SDL_MOUSEBUTTONDOWN)
                    printf("Hiciste click en el pixel (%i , %i)\n", e.button.x, e.button.y);
                    manejar_eventos_ganado(&e , &estado_actual, &juego);
                    break;

                case ESTADO_SALIENDO:
                    corriendo = false;
                    printf("\nSaliendo...\n");
                    break;
            }
        }

        //SDL_RenderClear(renderer);

        switch(estado_actual){
            case ESTADO_MENU:

                dibujar_menu(renderer , ventana , font , menu_items , menu_count , &seleccion);
                break;

            case ESTADO_JUGANDO:

                interfaz(renderer, font, &juego, &picords,filas,&rbutton);

                if(!juego.iniciado){

                    mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);
                    system("cls");
                    mapaImprimir(juego.mapa , filas , columnas);

                }

                casillaColocacion(juego.mapa, renderer, filas, columnas, &picords);
                break;
            case ESTADO_GANADO:
                interfazGanado(renderer, ventana, font, &juego, &picords,filas,&rbutton);
                break;

        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);

        /*

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

            case SDL_KEYDOWN:
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
               }
                break;
            }
        }
        */
    }
    fclose(archivoLog);
    return EJECUCION_OK;
}

void manejar_eventos_menu(SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int menu_count){

    switch(e->type){

        case SDL_KEYDOWN:

            switch(e->key.keysym.sym){

                case SDLK_UP:
                    *seleccion = (*seleccion - 1 + menu_count) % menu_count;
                    break;

                case SDLK_DOWN:
                    *seleccion = (*seleccion + 1) % menu_count;
                    break;

                case SDLK_RETURN:
                    switch(*seleccion){

                        case 0:
                            *estado_actual = ESTADO_JUGANDO;
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

void manejar_eventos_ganado(SDL_Event *e , EstadoJuego *estado_actual, Juego* juego){

    switch(e->type){
            case SDL_TEXTINPUT:
                // Actualizacion de nombreJugador al presionar una tecla
                if (strlen(juego->nombreJugador) + strlen(e->text.text) < 100)
                {
                    strcat(juego->nombreJugador, e->text.text);
                }
                break;
            case SDL_KEYDOWN:
                // Borrado de letra al presionar borrar
                if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(juego->nombreJugador) > 0)
                    juego->nombreJugador[strlen(juego->nombreJugador) - 1] = '\0';
                // Guardado de puntaje al presionar Enter
                if (e->key.keysym.sym == SDLK_RETURN && strlen(juego->nombreJugador) > 0)
                {
                    SDL_StopTextInput(); //Cierro la lectura de teclado
                    // Abrir archivos
                    FILE* aPuntuacion = abrirArchivo("puntuacion.txt", "rt");
                    if (!aPuntuacion) {
                        //fclose(archivoLog);
                        return ERROR_ARCHIVO;
                    }
                    FILE* aPuntuacionTemp = abrirArchivo("puntuacion.temp", "wt");
                    if (!aPuntuacionTemp) {
                        fclose(aPuntuacion);
                        //fclose(archivoLog);
                        return ERROR_ARCHIVO;
                    }
                    char linea[47];
                    char puntuacion[6];
                    int iterador = 0;
                    int guardado = 0;
                    Puntaje puntaje[MAX_PUNTAJES];
                    // Guardo todos los valores en un array
                    while(iterador < 20 && fgets(linea, sizeof(linea)+1, aPuntuacion) ){
                        char* iniPalabra = linea+6;
                        strncpy(puntuacion, linea, 5);
                        puntaje[iterador].puntos = atoi(puntuacion);
                        strncpy(puntaje[iterador].nombre, iniPalabra, 39);
                        iterador++;
                    }
                    // Guardo en orden en el archivo temp
                        for (size_t i = 0; i < iterador; i++)
                        {
                            if (!guardado && juego->puntaje < puntaje[i].puntos)
                            {
                                fprintf(aPuntuacionTemp, "%05d %-40s\n",  juego->puntaje,  juego->nombreJugador);
                                guardado = 1;
                            }
                            fprintf(aPuntuacionTemp, "%05d %-40s\n",  puntaje[i].puntos,  puntaje[i].nombre);
                    }
                    guardado = 0;
                    fclose(aPuntuacion);
                    fclose(aPuntuacionTemp);
                    *estado_actual = ESTADO_JUGANDO;

                    // Elimino puntuacion
                    if (remove("puntuacion.txt") != 0) {
                        perror("Error al eliminar el archivo fuente");
                        return ERROR_ELIMINACION_ARCHIVO;
                    }
                    // Renombro archivo temporal
                    if (rename("puntuacion.temp", "puntuacion.txt") != 0)
                    {
                        perror("Error renombrando puntuacion.temp");
                        printf("Error: %d\n", errno);
                        return ERROR_RENOMBRE_ARCHIVO;
                    }
                }
                break;
    }
}


void manejar_eventos_juego(SDL_Event *e , EstadoJuego *estado_actual , Juego* juego , Coord* minasCoord , int minas , Coord* picords , Coord* rbutton){

    int xG = ((e->button.x - (picords->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int yG = ((e->button.y - (picords->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int casillasLibresDeMinas = (juego->dimMapa * juego->dimMapa) - minas;

    switch(e->type){

        case SDL_MOUSEBUTTONDOWN:

            switch(e->button.button){

                case SDL_BUTTON_LEFT:

                    if((rbutton->x * TAM_PIXEL <= e->button.x && e->button.x <= (rbutton->x + 28) * TAM_PIXEL) &&
                       (rbutton->y * TAM_PIXEL <= e->button.y && e->button.y <= (rbutton->y + 28) * TAM_PIXEL)){
                           juego->iniciado = false;
                           break;
                       }

                    printf("Hiciste click en la casilla (%i , %i)\n",xG,yG);
                    casillaEstado(juego , minasCoord , minas , xG , yG);

                    if (juego->cantCasillasPresionadas == casillasLibresDeMinas){
                        puts("¡Ganaste el juego!");
                        SDL_StartTextInput();
                        juego->nombreJugador[0] = '\0';
                        *estado_actual = ESTADO_GANADO;
                    }
                    break;

                case SDL_BUTTON_RIGHT:
                    printf("Pusiste una bandera en la casilla (%i , %i)\n",xG,yG);
                    casillaBandera(juego , xG , yG);
                    break;
            }
            break;
    }

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
