#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h" //Header de estados
#include "juego.h"
#include "sonido.h" //Header de sonido

int leerConfiguracion(int *, int *, int *, char *);
int escribirArchivoLog(FILE *archivoLog, Log *log);
int leerPuntajes(Juego *juego);

int main(int argc, char *argv[])
{

    int filas = 0, columnas = 0, minasEnMapa = 0;
    char rutaFuente[100];
    Log log;
    Sonido sonidos;
    Sistema sistemas;
    // Creacion archivo Log
    FILE *archivoLog = abrirArchivo("partida.log", "w");

    setLog(&log, -1, -1, "Inicio del juego");
    escribirArchivoLog(archivoLog, &log);

    // Lectura del archivo de configuarcion
    leerConfiguracion(&filas, &columnas, &minasEnMapa, rutaFuente);

    // creacion archivo guardado

    Juego partidas[3];

    if (!archivoExiste(ARCHIVO_PARTIDAS))
    {
        inicializarPartidas(partidas);
        guardarPartidas(partidas, ARCHIVO_PARTIDAS);
        printf("Archivo creado e inicializado.\n");
    }


    // Iniciar SDL con funcion Video y audio
    if (SDL_Init(SDL_INIT_VIDEO) || SDL_Init(SDL_INIT_AUDIO))
    {
        puts("Error al iniciar SDL");
        return ERROR_CONFIGURACION;
    }

    if (iniciarSonido(&sonidos))
    {
        return ERROR_CONFIGURACION;
    }

    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error
    TTF_Init();
    sistemas.font = TTF_OpenFont(rutaFuente, 32);
    if (!sistemas.font)
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
    sistemas.ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    sistemas.renderer = SDL_CreateRenderer(sistemas.ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(sistemas.renderer, SDL_BLENDMODE_BLEND);

    //////////////////////////////////////////////////////////////////////
    const char *cargar_items[] = {
        "Partida 1",
        "Partida 2",
        "Partida 3"};
    const char *menu_items[] = {
        "Partida Rapida",
        "Cargar Partida",
        "Salir"};
    const char *pausa_items[] = {
        "Continuar",
        "Guardar",
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

    // Iniciacion de valores de mapa
    // mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);

    // Inicializar el juego
    juego.puntaje = 0;
    juego.cantCasillasPresionadas = 0;
    juego.cantMinasEnInterfaz = minasEnMapa;
    juego.dimMapa = filas;
    juego.finPartida = false;
    juego.start_time = time(NULL); // Iniciar el contador cuando inicia el juego
    juego.nombreJugador[0] = '\0';

    // Leer archivo de puntajes
    leerPuntajes(&juego);

    //-----------------------------------------------------

    if (cargarSonido("Sounds/sonidoMina.mp3", &sonidos.sonidoMina, 32) || cargarSonido("Sounds/sonidoCat.mp3", &sonidos.sonidoCat, 128) || cargarSonido("Sounds/sonidoClick.mp3", &sonidos.sonidoClick, 128) || cargarSonido("Sounds/sonidoBandera.mp3", &sonidos.sonidoBandera, 64) || cargarSonido("Sounds/sonidoPerdio.mp3", &sonidos.sonidoPerder, 32) || cargarSonido("Sounds/sonidoFlecha.mp3", &sonidos.sonidoFlecha, 32) || cargarSonido("Sounds/sonidoEnter.mp3", &sonidos.sonidoEnter, 32))
    {
        return ERROR_SONIDO;
    }

    if (cargarMusica("Sounds/musicaFondo.mp3", &sonidos.musicaFondo, 32) || cargarMusica("Sounds/musicaMenu.mp3", &sonidos.musicaMenu, 32))
    {
        return ERROR_SONIDO;
    }

    //-----------------------------------------------------

    SDL_Event e;       // Variable para registrar eventos
    int corriendo = 1; // Variable flag true para mantener corriendo el programa

    int boton, renderizarGanado = 0, fontSize = 16, casillasLibresDeMinas = (filas * columnas) - minasEnMapa;
    time_t current_time;

    // Variable para estados
    EstadoJuego estado_actual = ESTADO_MENU;
    EstadoJuego estado_anterior = ESTADO_MENU;
    int seleccion = 0;
    iniciarMusica(&sonidos.musicaMenu);

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
                manejar_eventos_menu(&e, &estado_actual, &sonidos, &seleccion, menu_count);
                estado_anterior = ESTADO_MENU;
                break;

            case ESTADO_JUGANDO:

                manejar_eventos_juego(&sistemas, &e, &estado_actual, estado_anterior, &juego, minasCoord, minasEnMapa, &picords, &rbutton, &sonidos);
                estado_anterior = ESTADO_JUGANDO;
                break;

            case ESTADO_GANADO:

                if (e.type == SDL_MOUSEBUTTONDOWN)
                    printf("Hiciste click en el pixel (%i , %i)\n", e.button.x, e.button.y);
                manejar_eventos_ganado(&e, &estado_actual, &juego);
                estado_anterior = ESTADO_GANADO;
                break;

            case ESTADO_SALIENDO:
                corriendo = false;
                printf("\nSaliendo...\n");
                break;
            case ESTADO_CARGAR:
                // estado_anterior = ESTADO_CARGAR; borrar
                manejar_eventos_slots(&e, &estado_actual, &sonidos, &seleccion, menu_count, &juego);
                estado_anterior = ESTADO_CARGAR;
                break;
            case ESTADO_PAUSA:
                manejar_eventos_pausa(&e, &estado_actual, &juego, &seleccion, &sonidos, menu_count, partidas);
                estado_anterior = ESTADO_PAUSA;
                break;
            case ESTADO_GUARDAR:
                manejar_eventos_slots(&e, &estado_actual, &sonidos, &seleccion, menu_count, &juego);
                estado_anterior = ESTADO_GUARDAR;
                break;
            }
        }

        // SDL_RenderClear(renderer);

        switch (estado_actual)
        {
        case ESTADO_MENU:
            dibujar_menu(sistemas.renderer, sistemas.ventana, sistemas.font, menu_items, menu_count, &seleccion);
            break;

        case ESTADO_JUGANDO:
            seleccion = 0;
            interfaz(sistemas.renderer, sistemas.font, &juego, &picords, filas, &rbutton);

            if (!juego.iniciado)
            {
                mapaReiniciar(sistemas.renderer, &picords, &juego, filas, columnas, minasCoord, minasEnMapa);
                system("cls");
                mapaImprimir(juego.mapa, filas, columnas);
            }

            casillaColocacion(juego.mapa, sistemas.renderer, filas, columnas, &picords);
            break;
        case ESTADO_GANADO:

            interfazGanado(sistemas.renderer, sistemas.ventana, sistemas.font, &juego, &picords, filas, &rbutton);
            break;
        case ESTADO_CARGAR:
            dibujar_menu(sistemas.renderer, sistemas.ventana, sistemas.font, cargar_items, menu_count, &seleccion);
            break;
        case ESTADO_PAUSA:
            interfazPausa(sistemas.renderer, sistemas.ventana, sistemas.font, &juego, &picords, juego.dimMapa, &rbutton, &seleccion, Mix_PlayingMusic(), pausa_items, menu_count);
            break;
        case ESTADO_GUARDAR:
            interfazPausa(sistemas.renderer, sistemas.ventana, sistemas.font, &juego, &picords, juego.dimMapa, &rbutton, &seleccion, Mix_PlayingMusic(), cargar_items, menu_count);
        }

        SDL_RenderPresent(sistemas.renderer);
        SDL_Delay(16);
    }
    // TODO cerrar todo SDL
    fclose(archivoLog);
    return EJECUCION_OK;
}

void manejar_eventos_menu(SDL_Event *e, EstadoJuego *estado_actual, Sonido *sonidos, int *seleccion, const int menu_count)
{
    switch (e->type)
    {

    case SDL_KEYDOWN:

        switch (e->key.keysym.sym)
        {

        case SDLK_UP:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion - 1 + menu_count) % menu_count;
            break;

        case SDLK_DOWN:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion + 1) % menu_count;
            break;

        case SDLK_RETURN:
            Mix_PlayChannel(-1, sonidos->sonidoEnter, 0);

                switch (*seleccion)
                {

                case 0:
                    *estado_actual = ESTADO_JUGANDO;
                    iniciarMusica(&sonidos->musicaFondo);
                    break;

                case 1:
                    *seleccion = 0;
                    *estado_actual = ESTADO_CARGAR;
                    break;

                case 2:
                    *estado_actual = ESTADO_SALIENDO;
                    break;
                }
                break;

        }
    }
}

void manejar_eventos_slots(SDL_Event *e, EstadoJuego *estado_actual, Sonido *sonidos, int *seleccion, const int menu_count, Juego *juego)
{
    switch (e->type)
    {

    case SDL_KEYDOWN:

        switch (e->key.keysym.sym)
        {

        case SDLK_UP:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion - 1 + menu_count) % menu_count;
            break;

        case SDLK_DOWN:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion + 1) % menu_count;
            break;

        case SDLK_RETURN:
            Mix_PlayChannel(-1, sonidos->sonidoEnter, 0);
            switch (*estado_actual)
            {
            case ESTADO_CARGAR:

                cargarDesdeSlot(juego, *seleccion);
                *estado_actual = ESTADO_JUGANDO;
                iniciarMusica(&sonidos->musicaFondo);

                break;
            case ESTADO_GUARDAR:

                guardarEnSlot(juego, *seleccion);
                *estado_actual = ESTADO_PAUSA;
                *seleccion = 0;
            break;
            }
            break;
        }
    }
}

void manejar_eventos_ganado(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego)
{
    int TAMX = TAM_PIXEL * (juego->dimMapa * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (juego->dimMapa * PIXELES_X_LADO + 4 + 3 * 8 + 28);

    // X e Y de boton cerrado
    int x = (TAMX / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12;
    int y = (TAMY / 2) - (TAMY_GANADO / 2) + 15 + 4;

    switch (e->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if ((x <= e->button.x && e->button.x <= (x + 30)) &&
            (y <= e->button.y && e->button.y <= (y + 30)) &&
            (e->button.button == SDL_BUTTON_LEFT))
        {
            *estado_actual = ESTADO_JUGANDO;
        }
        //         rectanguloLlenoAbsoluto(renderer, RR, (win_width / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12, pcords->y + 15 + 4, TAM_BOTON_CERRADO, TAM_BOTON_CERRADO);
        break;
    case SDL_TEXTINPUT:
        // Actualizacion de nombreJugador al presionar una tecla
        if (strlen(juego->nombreJugador) + strlen(e->text.text) <= 40)
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
            SDL_StopTextInput(); // Cierro la lectura de teclado

            // Abrir archivo
            FILE *aPuntuacionTemp = abrirArchivo("puntuacion.temp", "wt");
            if (!aPuntuacionTemp)
            {
                // fclose(archivoLog);
                return ERROR_ARCHIVO;
            }

            // Preparar el nuevo puntaje
            Puntaje nuevo;
            strncpy(nuevo.nombre, juego->nombreJugador, 40);
            nuevo.puntos = juego->puntaje;

            int i = juego->totalPuntajes - 1; // Para que la primera iteracion no se haga

            if (i + 1 == MAX_PUNTAJES)
                i--;

            // Si el tiempo nuevo es menor al ultimo registro itera hacia arriba desplazando los registros hacia abajo
            while (i >= 0 && juego->puntajes[i].puntos > nuevo.puntos)
            {
                juego->puntajes[i + 1] = juego->puntajes[i];
                i--;
            }
            // Insertar nuevo puntaje en posición
            juego->puntajes[i + 1] = nuevo;
            if (juego->totalPuntajes != MAX_PUNTAJES)
                juego->totalPuntajes++;

            // Escribir los puntajes al archivo temporal
            for (int i = 0; i < juego->totalPuntajes && i < MAX_PUNTAJES; i++)
            {
                fprintf(aPuntuacionTemp, "%05d %-40s\n", juego->puntajes[i].puntos, juego->puntajes[i].nombre);
            }

            fclose(aPuntuacionTemp);
            *estado_actual = ESTADO_MENU;

            // Elimino puntuacion
            if (remove("puntuacion.txt") != 0)
            {
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

void manejar_eventos_pausa(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego, int *seleccion, Sonido *sonidos, const int menu_count, Juego partidas[3])
{

    char *slots[3] = {
        "slot 1",
        "slot 2",
        "slot 3"};
    int TAMX = TAM_PIXEL * (juego->dimMapa * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (juego->dimMapa * PIXELES_X_LADO + 4 + 3 * 8 + 28);

    // X e Y de boton cerrado
    int x = (TAMX / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12;
    int y = (TAMY / 2) - (TAMY_GANADO / 2) + 15 + 4;

    switch (e->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if ((x <= e->button.x && e->button.x <= (x + 30)) &&
            (y <= e->button.y && e->button.y <= (y + 30)) &&
            (e->button.button == SDL_BUTTON_LEFT))
        {
            Mix_PlayingMusic() ? Mix_HaltMusic() : Mix_ResumeMusic();
        }
        break;
    case SDL_KEYDOWN:

        switch (e->key.keysym.sym)
        {

        case SDLK_UP:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion - 1 + menu_count) % menu_count;
            break;

        case SDLK_DOWN:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion + 1) % menu_count;
            break;

        case SDLK_RETURN:
            Mix_PlayChannel(-1, sonidos->sonidoEnter, 0);
            switch (*seleccion)
            {
            case 0:
                *estado_actual = ESTADO_JUGANDO;
                break;

            case 1:
                *seleccion = 0;
                *estado_actual = ESTADO_GUARDAR;
                break;

            case 2:
                *seleccion = 0;
                *estado_actual = ESTADO_MENU;
                iniciarMusica(&sonidos->musicaMenu);
                juego->finPartida = true;
                break;
            }
            break;
        }
        break;
    }
}

void manejar_eventos_juego(Sistema *sistemas, SDL_Event *e, EstadoJuego *estado_actual, EstadoJuego estado_anterior, Juego *juego, Coord *minasCoord, int minas, Coord *picords, Coord *rbutton, Sonido *sonidos)
{
    if(estado_anterior == ESTADO_MENU)
        mapaReiniciar(sistemas->renderer, picords, juego, juego->dimMapa, juego->dimMapa, minasCoord, minas);

    Casilla **mapa = juego->mapa;

    int xG = ((e->button.x - (picords->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int yG = ((e->button.y - (picords->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int casillasLibresDeMinas = (juego->dimMapa * juego->dimMapa) - minas;
    // guardado del boton anterior antes de nuevo evento
    int boton = e->button.button;
    int tecla = e->key.keysym.sym;
    switch (e->type)
    {
        EventoClick handlerClick;
    case SDL_MOUSEBUTTONDOWN:

        if ((rbutton->x * TAM_PIXEL <= e->button.x && e->button.x <= (rbutton->x + 28) * TAM_PIXEL) &&
            (rbutton->y * TAM_PIXEL <= e->button.y && e->button.y <= (rbutton->y + 28) * TAM_PIXEL) && (boton == SDL_BUTTON_LEFT))
        {
            Mix_PlayChannel(-1, sonidos->sonidoCat, 0);

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
                        handlerClick(juego, sonidos, xG, yG, minasCoord, minas);
                        continue;
                    }
                    SDL_Delay(1);
                }
            }
            else
            {
                handlerClick = (boton == SDL_BUTTON_LEFT) ? handlerClickIzquierdo : handlerClickDerecho;
                handlerClick(juego, sonidos, xG, yG, minasCoord, minas);
                if (juego->cantCasillasPresionadas == casillasLibresDeMinas)
                {
                    puts("¡Ganaste el juego!");
                    SDL_StartTextInput();
                    juego->nombreJugador[0] = '\0';
                    *estado_actual = ESTADO_GANADO;
                }
            }
        }
        break;
    case SDL_KEYDOWN:
        if (tecla == SDLK_p)
        {

            *estado_actual = ESTADO_PAUSA;
        }
        break;
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

int leerPuntajes(Juego *juego)
{
    // Abrir archivos
    FILE *aPuntuacion = abrirArchivo("puntuacion.txt", "rt");
    if (!aPuntuacion)
    {
        // fclose(archivoLog);
        return ERROR_ARCHIVO;
    }
    char linea[47];
    int total = 0;
    // Guardo los puntajes en array
    while (fgets(linea, sizeof(linea) + 1, aPuntuacion) && total < MAX_PUNTAJES)
    {
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
