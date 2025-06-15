#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"

int leerConfiguracion(int *, int *, int *, char *);
int escribirArchivoLog(FILE *archivoLog, Log *log);
int leerPuntajes(Juego* juego);

int main(int argc, char *argv[])
{

    int filas = 0, columnas = 0, minasEnMapa = 0;
    char rutaFuente[100];
    // Creacion archivo log

    FILE *archivoLog = fopen("partida.log", "w");
    if (!archivoLog)
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
    int TAMX = TAM_PIXEL * (columnas * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (filas * PIXELES_X_LADO + 4 + 3 * 8 + 28);
    // Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //////////////////////////////////////////////////////////////////////

    const char *menu_items[] = {
        "Nueva Partida",
        "Cargar Partida",
        "Salir"};

    int menu_count = sizeof(menu_items) / sizeof(menu_items[0]);

    //////////////////////////////////////////////////////////////////////

    // fondoColor(renderer); //Funcion para establecer fondo del render color defecto

    Coord picords = {0, 0};
    Coord rbutton = {0, 0};
    // interfaz(renderer , &picords , filas , &rbutton); //Funcion para colocar la interfaz

    //////////////////////////////////////////////////////////////////////

    // Creacion de matriz mapa
    Casilla **mapa = matrizCrear(filas, columnas, sizeof(Casilla));

    // Vector de coordenadas para las minas
    Coord minasCoord[minasEnMapa];

    Juego juego;
    juego.mapa = mapa;
    juego.iniciado = false;
    juego.senialRender = 1;

    // Leer archivo de puntajes
    leerPuntajes(&juego);

    // Iniciacion de valores de mapa
    // mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);

    // Imprimir mapa
    // mapaImprimir(juego.mapa , filas , columnas);

    //////////////////////////////////////////////////////////////////////

    SDL_Event e;       // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    // Variable para estados
    EstadoJuego estado_actual = ESTADO_MENU;
    int seleccion = 0;

    // Creacion de struct para reducir parametros en funciones comunes
    Graficos graficos;
    graficos.ventana = ventana;
    graficos.renderer = renderer;
    graficos.font = font;
    graficos.fontSize = 16;
    graficos.piCord = &picords;
    graficos.G = 2; // Grosor
    graficos.pad = graficos.G * 4;
    graficos.altoC = 28;
    graficos.tamXVentana = TAMX;
    graficos.tamYVentana = TAMY;
    // While para mantener el programa corriendo
    while (corriendo)
    {

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                corriendo = false;
                printf("\n---Cerrando Ventana---\n");
            }

            switch (estado_actual)
            {
            case ESTADO_MENU:
                juego.senialRender=1;
                manejar_eventos_menu(&e, &estado_actual, &seleccion, menu_count);
                break;

            case ESTADO_JUGANDO:
                manejar_eventos_juego(&e, &estado_actual, &juego, &minasCoord, minasEnMapa, &picords, &rbutton);
                break;

            case ESTADO_GANADO:
                manejar_eventos_ganado(&e, &estado_actual, &juego);
                break;

            case ESTADO_SALIENDO:
                corriendo = false;
                printf("\nSaliendo...\n");
                break;
            default: break;
            }
        }

        if(estado_actual==ESTADO_JUGANDO || estado_actual == ESTADO_GANADO)tiempoYbombas(&graficos, &juego);

        if(juego.senialRender){
            switch (estado_actual)
            {
            case ESTADO_MENU:

                dibujar_menu(&graficos, menu_items, menu_count, &seleccion);
                juego.senialRender=0;
                break;

            case ESTADO_JUGANDO:

                if (!juego.iniciado)
                {

                    mapaReiniciar(&juego, filas, columnas, &minasCoord, minasEnMapa);
                    system("cls");
                    graficos.anchoM = juego.dimMapa * PIXELES_X_LADO + 4;
                    mapaImprimir(juego.mapa, filas, columnas);
                }

                interfaz(&graficos, &juego, &rbutton);
                casillaColocacion(juego.mapa, renderer, filas, columnas, &picords);
                juego.senialRender=0;
                break;
            case ESTADO_GANADO:
                interfazGanado(&graficos, &juego);
                juego.senialRender=0;
                break;
            default: break;
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);

    }
    // TODO cerrar todo SDL
    fclose(archivoLog);
    return EJECUCION_OK;
}

void manejar_eventos_menu(SDL_Event *e, EstadoJuego *estado_actual, int *seleccion, const int menu_count)
{

    switch (e->type)
    {

    case SDL_KEYDOWN:

        switch (e->key.keysym.sym)
        {

        case SDLK_UP:
            *seleccion = (*seleccion - 1 + menu_count) % menu_count;
            break;

        case SDLK_DOWN:
            *seleccion = (*seleccion + 1) % menu_count;
            break;

        case SDLK_RETURN:
            switch (*seleccion)
            {

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
    int TAMX = TAM_PIXEL * (juego->dimMapa * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (juego->dimMapa * PIXELES_X_LADO + 4 + 3 * 8 + 28);

    // X e Y de boton cerrado
    int x=(TAMX/2)+(TAMX_GANADO/2)- 15 - 20 - 12;
    int y=(TAMY/2)-(TAMY_GANADO/2)+ 15 + 4;

    switch(e->type){
            case SDL_MOUSEBUTTONDOWN:
                juego->senialRender=1;
                if ((x <= e->button.x && e->button.x <= (x + 30)) &&
                    (y <= e->button.y && e->button.y <= (y + 30)) &&
                    (e->button.button == SDL_BUTTON_LEFT))
                {
                    *estado_actual=ESTADO_JUGANDO;
                }
    //         rectanguloLlenoAbsoluto(renderer, RR, (win_width / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12, pcords->y + 15 + 4, TAM_BOTON_CERRADO, TAM_BOTON_CERRADO);
                break;
            case SDL_TEXTINPUT:
                juego->senialRender=1;
                // Actualizacion de nombreJugador al presionar una tecla
                if (strlen(juego->nombreJugador) + strlen(e->text.text) <= 40)
                {
                    strcat(juego->nombreJugador, e->text.text);
                }
                break;
            case SDL_KEYDOWN:
                juego->senialRender=1;
                // Borrado de letra al presionar borrar
                if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(juego->nombreJugador) > 0)
                    juego->nombreJugador[strlen(juego->nombreJugador) - 1] = '\0';
                // Guardado de puntaje al presionar Enter
                if (e->key.keysym.sym == SDLK_RETURN && strlen(juego->nombreJugador) > 0)
                {
                    SDL_StopTextInput(); //Cierro la lectura de teclado

                    // Abrir archivo
                    FILE* aPuntuacionTemp = abrirArchivo("puntuacion.temp", "wt");
                    if (!aPuntuacionTemp) {
                        //fclose(archivoLog);
                        return ERROR_ARCHIVO;
                    }

                    // Preparar el nuevo puntaje
                    Puntaje nuevo;
                    strncpy(nuevo.nombre, juego->nombreJugador, 40);
                    nuevo.puntos = juego->puntaje;

                    int i = juego->totalPuntajes - 1; // Para que la primera iteracion no se haga

                    if(i + 1 == MAX_PUNTAJES)
                        i--;

                    // Si el tiempo nuevo es menor al ultimo registro itera hacia arriba desplazando los registros hacia abajo
                    while (i >= 0 && juego->puntajes[i].puntos > nuevo.puntos) {
                        juego->puntajes[i + 1] = juego->puntajes[i];
                        i--;
                    }
                    // Insertar nuevo puntaje en posición
                    juego->puntajes[i + 1] = nuevo;
                    if(juego->totalPuntajes != MAX_PUNTAJES)
                        juego->totalPuntajes++;

                    // Escribir los puntajes al archivo temporal
                    for (int i = 0; i < juego->totalPuntajes && i < MAX_PUNTAJES; i++) {
                        fprintf(aPuntuacionTemp, "%05d %-40s\n", juego->puntajes[i].puntos, juego->puntajes[i].nombre);
                    }

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

void manejar_eventos_juego(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego, Coord *minasCoord, int minas, Coord *picords, Coord *rbutton)
{
    Casilla **mapa = juego->mapa;

    int xG = ((e->button.x - (picords->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int yG = ((e->button.y - (picords->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int casillasLibresDeMinas = (juego->dimMapa * juego->dimMapa) - minas;

    if (e->type == SDL_MOUSEBUTTONDOWN)
    {
        juego->senialRender=1;
        int boton = e->button.button; // guardado del boton anterior antes de nuevo evento
        EventoClick handlerClick;

        if ((rbutton->x * TAM_PIXEL <= e->button.x && e->button.x <= (rbutton->x + 28) * TAM_PIXEL) &&
            (rbutton->y * TAM_PIXEL <= e->button.y && e->button.y <= (rbutton->y + 28) * TAM_PIXEL) && (boton == SDL_BUTTON_LEFT))
        {

            juego->iniciado = false;
        }
        else
        {
            if (mapa[yG][xG].presionada &&
                mapa[yG][xG].estado > 0)
            {

                handlerClick = clickDoble;
                Uint32 tiempoDeEspera = SDL_GetTicks() + 100;
                while (SDL_GetTicks() < tiempoDeEspera)
                {
                    if (SDL_PollEvent(e) && e->type == SDL_MOUSEBUTTONDOWN &&
                        e->button.button != boton)
                    {
                        handlerClick(juego, xG, yG, minasCoord, minas);
                        continue;
                    }
                    SDL_Delay(1);
                }
            }
            else
            {
                handlerClick = (boton == SDL_BUTTON_LEFT) ? handlerClickIzquierdo : handlerClickDerecho;
                handlerClick(juego, xG, yG, minasCoord, minas);
                if (juego->cantCasillasPresionadas == casillasLibresDeMinas)
                {
                    puts("¡Ganaste el juego!");
                    SDL_StartTextInput();
                    juego->nombreJugador[0] = '\0';
                    *estado_actual = ESTADO_GANADO;
                }
            }
        }
    }
}

int leerConfiguracion(int *filas, int *columnas, int *minasEnMapa, char *rutaFuente)
{
    FILE *config = fopen("buscaminas.conf", "r+t");

    if (!config)
    {
        puts("Error al abrir archivo de configuracion. Cerrando juego...");
        return ERROR_ARCHIVO;
    }

    if (fscanf(config, "DIMENSION_MAPA = %d\n", filas) != 1)
    {
        puts("Error al leer DIMENSION_MAPA.");
        fclose(config);
        return ERROR_ARCHIVO;
    }

    if (*filas < 8 || *filas > 32)
    {
        puts("Error de configuracion DIMENSION_MAPA valores validos entre 8 y 32.");
        fclose(config);
        return ERROR_CONFIGURACION;
    }

    *columnas = *filas; // Mapa cuadrado siempre

    char minasTexto[5];
    if (fscanf(config, "CANTIDAD_MINAS = %s\n", minasTexto) != 1)
    {
        puts("Error al leer CANTIDAD_MINAS.");
        fclose(config);
        return ERROR_ARCHIVO;
    }

    char *porcentaje = strchr(minasTexto, '%');
    *minasEnMapa = atoi(minasTexto);

    // Lo convierto a un porcentual del mapa
    if (porcentaje)
        *minasEnMapa = round(((*filas) * (*columnas)) * ((float)*minasEnMapa / 100));

    if (fscanf(config, "RUTA_FUENTE = %s\n", rutaFuente) != 1)
    {
        puts("Error al leer RUTA_FUENTE.");
        fclose(config);
        return ERROR_ARCHIVO;
    }
    // printf("%d, %d, %s", *filas, *minasEnMapa, rutaFuente);
    fclose(config);

    return 0;
}

int escribirArchivoLog(FILE *archivoLog, Log *log)
{
    if (log->coordXY[0] == -1 && log->coordXY[1] == -1)
    {
        fprintf(archivoLog, "[%d-%d-%d %02d:%02d:%02d] %-15s\n",
                log->fechaHora.tm_year + 1900, log->fechaHora.tm_mon + 1, log->fechaHora.tm_mday,
                log->fechaHora.tm_hour, log->fechaHora.tm_min, log->fechaHora.tm_sec, log->tipoEvento);
    }
    else
    {
        fprintf(archivoLog, "[%d-%d-%d %02d:%02d:%02d] %-15s | coordenadas: (%d , %d)\n",
                log->fechaHora.tm_year + 1900, log->fechaHora.tm_mon + 1, log->fechaHora.tm_mday,
                log->fechaHora.tm_hour, log->fechaHora.tm_min, log->fechaHora.tm_sec, log->tipoEvento,
                log->coordXY[0], log->coordXY[1]);
    }
    return 0;
}

int leerPuntajes(Juego* juego){
    // Abrir archivos
    FILE* aPuntuacion = abrirArchivo("puntuacion.txt", "rt");
    if (!aPuntuacion) {
        //fclose(archivoLog);
        return ERROR_ARCHIVO;
    }
    char linea[47];
    int total = 0;
    // Guardo los puntajes en array
    while (fgets(linea, sizeof(linea)+1, aPuntuacion) && total < MAX_PUNTAJES) {
        strncpy(juego->puntajes[total].nombre, linea + 6, 40); // +6 por el formato del archivo, 5 de tiempo y 1 de espacio
        juego->puntajes[total].nombre[39] = '\0';
        juego->puntajes[total].puntos = atoi(linea);
        total++;
    }
    //*juego->puntajes = puntajes;
    juego->totalPuntajes = total;
    fclose(aPuntuacion);
    return 0;
}
