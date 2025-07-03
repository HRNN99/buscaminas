/**

    Apellido: Ortega, Marco Antonio
    DNI: 44108566
    Entrega: Si

    Apellido: Villalba, Hernan Agustin
    DNI: 42057001
    Entrega: Si

    Apellido: Caputo, Franco Gustavo
    DNI: 42949130
    Entrega: Si

**/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>

#include "dibujos.h"
#include "juego.h"
#include "sonido.h" //Header de sonido

bool linea_ignorable(const char *linea);
void crear_dificultades_default(const char* archivo);
int cargar_dificultades(const char *archivo, Dificultad *difs, int num_dif);
int escribirArchivoLog(FILE *archivoLog, Log *log);
int leerPuntajes(Juego *juego);

int main(int argc, char *argv[])
{

    //////////////////////////////////////////////////////////////////////

    // Iniciar SDL con funcion Video y audio
    if (SDL_Init(SDL_INIT_VIDEO) || SDL_Init(SDL_INIT_AUDIO))
    {
        puts("Error al iniciar SDL");
        return ERROR_CONFIGURACION;
    }

    Sonido sonidos;
    if (iniciarSonido(&sonidos))
    {
        return ERROR_CONFIGURACION;
    }

    //////////////////////////////////////////////////////////////////////

    Juego partidas[3];

    // Tamaño de ancho y altura de la ventana, utilizo 1 sola variable ya que sera cuadrada
    int TAMX = TAM_PIXEL * (16 * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (16 * PIXELES_X_LADO + 4 + 3 * 8 + 28);
    // Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow("Buscaminas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //////////////////////////////////////////////////////////////////////

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

    //////////////////////////////////////////////////////////////////////

    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("fnt/digital-7.ttf", 32);

    if (!font)
    {

        printf("Error cargando fuente: %s\n", TTF_GetError());
        return ERROR_FUENTE;
    }

    //////////////////////////////////////////////////////////////////////
    const char *menu_items[] = {
        "Nueva Partida",
        "Cargar Partida",
        "Salir"};
    int menu_count = sizeof(menu_items) / sizeof(menu_items[0]);
    int seleccion_menu = 0;

    const char *dificultad_items[] = {
        "Facil",
        "Intermedio",
        "Dificil"};
    int dificultad_count = sizeof(dificultad_items) / sizeof(dificultad_items[0]);
    int seleccion_dificultad = 0;

    const char *cargar_items[] = {
        "Slot 1",
        "Slot 2",
        "Slot 3"};
    int cargar_count = sizeof(cargar_items) / sizeof(cargar_items[0]);
    int seleccion_cargar = 0;
    int seleccion_guardar = 0;

    const char *pausa_items[] = {
        "Continuar",
        "Guardar",
        "Salir"};
    int pausa_count = sizeof(pausa_items) / sizeof(pausa_items[0]);
    int seleccion_pausa = 0;
    Dificultad dificultades[dificultad_count];

    cargar_dificultades("buscaminas.conf", dificultades, dificultad_count);

    //////////////////////////////////////////////////////////////////////

    Juego juego;
    juego.mapa = NULL;
    juego.iniciado = false;
    juego.senialRender = 1;
    juego.log = archivoLog;
    // Inicializar el juego
    juego.puntaje = 0;
    juego.cantCasillasPresionadas = 0;
    // juego.cantMinasEnInterfaz = minasEnMapa;
    // juego.dimMapa = filas;
    juego.finPartida = false;
    juego.start_time = time(NULL); // Iniciar el contador cuando inicia el juego
    juego.nombreJugador[0] = '\0';

    // Leer archivo de puntajes
    leerPuntajes(&juego);

    //////////////////////////////////////////////////////////////////////

    if (cargarSonido("snd/sonidoMina.mp3", &sonidos.sonidoMina, 32) || cargarSonido("snd/sonidoCat.mp3", &sonidos.sonidoCat, 128) || cargarSonido("snd/sonidoClick.mp3", &sonidos.sonidoClick, 128) || cargarSonido("snd/sonidoBandera.mp3", &sonidos.sonidoBandera, 64) || cargarSonido("snd/sonidoPerdio.mp3", &sonidos.sonidoPerder, 32) || cargarSonido("snd/sonidoFlecha.mp3", &sonidos.sonidoFlecha, 32) || cargarSonido("snd/sonidoEnter.mp3", &sonidos.sonidoEnter, 32))
    {
        return ERROR_SONIDO;
    }

    if (cargarMusica("snd/musicaFondo.mp3", &sonidos.musicaFondo, 32) || cargarMusica("snd/musicaMenu.mp3", &sonidos.musicaMenu, 32))
    {
        return ERROR_SONIDO;
    }

    //////////////////////////////////////////////////////////////////////

    Coord picords = {0, 0};
    Coord rbutton = {0, 0};

    int corriendo = true;                    // Variable flag true para mantener corriendo el programa
    SDL_Event e;                             // Variable para registrar eventos
    EstadoJuego estado_actual = ESTADO_MENU; // Variable para estados
    EstadoJuego estado_anterior;
    iniciarMusica(&sonidos.musicaMenu);

    //////////////////////////////////////////////////////////////////////

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

    //////////////////////////////////////////////////////////////////////

    SDL_Texture *fondo = cargarFondo(renderer, "img/fondo_menu.bmp");

    // While para mantener el programa corriendo
    while (corriendo)
    {

        //////////////////////////////////////////////////////////////////////

        while (SDL_PollEvent(&e))
        {

            if (e.type == SDL_QUIT)
            {

                corriendo = false;
                matrizDestruir(juego.mapa, juego.dificultad.dimension);
                printf("\n---Cerrando Ventana---\n");
            }

            switch (estado_actual)
            {
            case ESTADO_MENU:
                juego.senialRender = 1;
                estado_anterior = estado_actual;
                manejar_eventos_menu(&e, &estado_actual, &seleccion_menu, menu_count, &sonidos);
                break;

            case ESTADO_DIFICULTAD:
                juego.senialRender = 1;
                estado_anterior = estado_actual;
                manejar_eventos_dificultad(&graficos, &e, &estado_actual, &seleccion_dificultad, dificultad_count, &juego, dificultades, ventana);
                break;

            case ESTADO_JUGANDO:
                estado_anterior = estado_actual;
                manejar_eventos_juego(&graficos, &e, &estado_actual, &estado_anterior, &juego, &picords, &rbutton, &sonidos);
                break;

            case ESTADO_GANADO:
                estado_anterior = estado_actual;
                manejar_eventos_ganado(&e, &estado_actual, &juego);
                break;

            case ESTADO_SALIENDO:
                estado_anterior = estado_actual;
                corriendo = false;
                matrizDestruir(juego.mapa, juego.dificultad.dimension);
                printf("\nSaliendo...\n");
                break;
            case ESTADO_CARGAR:
                juego.senialRender = 1;
                manejar_eventos_slots(&graficos, &e, &estado_actual, &sonidos, &seleccion_cargar, menu_count, &juego);
                estado_anterior = ESTADO_CARGAR;
                break;
            case ESTADO_PAUSA:
                juego.senialRender = 1;
                manejar_eventos_pausa(&e, &estado_actual, &juego, &seleccion_pausa, &sonidos, menu_count, partidas);
                estado_anterior = ESTADO_PAUSA;
                break;
            case ESTADO_GUARDAR:
                juego.senialRender = 1;
                manejar_eventos_slots(&graficos, &e, &estado_actual, &sonidos, &seleccion_guardar, menu_count, &juego);
                estado_anterior = ESTADO_GUARDAR;
                break;
            default:
                break;
            }
        }

        //////////////////////////////////////////////////////////////////////
        if (!(estado_actual == ESTADO_GANADO) && (estado_actual == ESTADO_JUGANDO))
            tiempoYbombas(&graficos, &juego);

        // SDL_RenderClear(renderer);
        if (juego.senialRender)
        {
            switch (estado_actual)
            {

            case ESTADO_MENU:
                // dibujarFondo(renderer , fondo);
                dibujar_menu(&graficos, menu_items, menu_count, &seleccion_menu, fondo);
                juego.senialRender = 0;
                break;

            case ESTADO_DIFICULTAD:
                dibujar_menu(&graficos, dificultad_items, dificultad_count, &seleccion_dificultad, fondo);
                juego.senialRender = 0;
                break;

            case ESTADO_JUGANDO:

                if (!juego.iniciado)
                {
                    mapaReiniciar(&juego);
                    system("cls");
                    // Modifico el ancho del Mapa para manejo de interfaz
                    graficos.anchoM = juego.dificultad.dimension * PIXELES_X_LADO + 4;
                    mapaImprimir(juego.mapa, juego.dificultad.dimension, juego.dificultad.dimension);
                }
                interfaz(&graficos, &juego, &rbutton);
                casillaColocacion(renderer, juego.mapa, juego.dificultad.dimension, &picords);
                juego.senialRender = 0;
                break;

            case ESTADO_GANADO:
                interfazGanado(&graficos, &juego);
                juego.senialRender = 0;
                break;
            case ESTADO_CARGAR:
                dibujar_menu(&graficos, cargar_items, cargar_count, &seleccion_cargar, fondo);
                juego.senialRender = 0;
                break;
            case ESTADO_PAUSA:
                interfazPausa(graficos.renderer, graficos.ventana, graficos.font, &juego, &picords, juego.dificultad.dimension, &rbutton, &seleccion_pausa, !Mix_PausedMusic(), pausa_items, pausa_count);
                juego.senialRender = 0;
                break;
            case ESTADO_GUARDAR:
                interfazPausa(graficos.renderer, graficos.ventana, graficos.font, &juego, &picords, juego.dificultad.dimension, &rbutton, &seleccion_guardar, !Mix_PausedMusic(), cargar_items, cargar_count);
                juego.senialRender = 0;
                break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    //////////////////////////////////////////////////////////////////////

    if (!juego.mapa)
        matrizDestruir(juego.mapa, juego.dificultad.dimension);

    FinalizarSDL(ventana, renderer, font, EXIT_SUCCESS, archivoLog); // Funcion para la finalizacion de SDL y sus componentes

    SDL_DestroyTexture(fondo);

    return EJECUCION_OK;
}

//////////////////////////////////////////////////////////////////////

void manejar_eventos_slots(Graficos *graficos, SDL_Event *e, EstadoJuego *estado_actual, Sonido *sonidos, int *seleccion, const int menu_count, Juego *juego)
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
                if (cargarDesdeSlot(graficos, juego, *seleccion))
                {
                    graficos->anchoM = juego->dificultad.dimension * PIXELES_X_LADO + 4;
                    graficos->tamXVentana = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 20);
                    graficos->tamYVentana = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);
                    SDL_SetWindowSize(graficos->ventana, graficos->tamXVentana, graficos->tamYVentana);
                    iniciarMusica(&sonidos->musicaFondo);
                    *estado_actual = ESTADO_JUGANDO;
                }
                else
                {

                    *estado_actual = ESTADO_MENU;
                }
                break;
            case ESTADO_GUARDAR:
                guardarEnSlot(juego, *seleccion);
                *estado_actual = ESTADO_PAUSA;
                break;
            default:
                break;
            }

        default:
            break;
        }
    }
}

void manejar_eventos_pausa(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego, int *seleccion, Sonido *sonidos, const int menu_count, Juego partidas[3])
{
    int TAMX = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);

    int x = (TAMX / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12;
    int y = (TAMY / 2) - (TAMY_GANADO / 2) + 15 + 4;

    switch (e->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if ((x <= e->button.x && e->button.x <= (x + 30)) &&
            (y <= e->button.y && e->button.y <= (y + 30)) &&
            (e->button.button == SDL_BUTTON_LEFT))
        {
            if (Mix_PlayingMusic())
            {
                if (Mix_PausedMusic())
                    Mix_ResumeMusic();
                else
                    Mix_PauseMusic();
            }
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

int manejar_eventos_menu(SDL_Event *e, EstadoJuego *estado_actual, int *seleccion, const int items_count, Sonido *sonidos)
{

    switch (e->type)
    {

    case SDL_KEYDOWN:

        switch (e->key.keysym.sym)
        {

        case SDLK_UP:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion - 1 + items_count) % items_count;
            break;

        case SDLK_DOWN:
            Mix_PlayChannel(-1, sonidos->sonidoFlecha, 0);
            *seleccion = (*seleccion + 1) % items_count;
            break;

        case SDLK_RETURN:
            Mix_PlayChannel(-1, sonidos->sonidoEnter, 0);
            switch (*seleccion)
            {

            case 0:
                *estado_actual = ESTADO_DIFICULTAD;
                iniciarMusica(&sonidos->musicaFondo);
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
    return 0;
}

int manejar_eventos_dificultad(Graficos *graficos, SDL_Event *e, EstadoJuego *estado_actual, int *seleccion, const int items_count, Juego *juego, Dificultad *difs, SDL_Window *ventana)
{

    switch (e->type)
    {

    case SDL_KEYDOWN:

        switch (e->key.keysym.sym)
        {

        case SDLK_UP:
            *seleccion = (*seleccion - 1 + items_count) % items_count;
            break;

        case SDLK_DOWN:
            *seleccion = (*seleccion + 1) % items_count;
            break;

        case SDLK_RETURN:
            switch (*seleccion)
            {
                Log log;
            case 0:
                // juego->dificultad.nombre = difs[0].nombre;
                juego->dificultad.dimension = difs[0].dimension;
                juego->dificultad.cantidad_minas = difs[0].cantidad_minas;
                juego->mapa = (Casilla **)matrizCrear(juego->dificultad.dimension, juego->dificultad.dimension, sizeof(Casilla)); // Mover a otro lado despues
                juego->minasCoord = (Coord *)matrizCrear(juego->dificultad.cantidad_minas, 0, sizeof(Coord));
                *estado_actual = ESTADO_JUGANDO;
                graficos->tamXVentana = TAM_PIXEL * (difs[0].dimension * PIXELES_X_LADO + 20);
                graficos->tamYVentana = TAM_PIXEL * (difs[0].dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);
                graficos->anchoM = juego->dificultad.dimension * PIXELES_X_LADO + 4;
                SDL_SetWindowSize(ventana, graficos->tamXVentana, graficos->tamYVentana);
                // Logeo de seleccion
                setLog(&log, -1, -1, "Dificultad cargada: Facil.");
                mapaReiniciar(juego);
                escribirArchivoLog(juego->log, &log);
                break;

            case 1:
                // juego->dificultad.nombre = difs[1].nombre;
                juego->dificultad.dimension = difs[1].dimension;
                juego->dificultad.cantidad_minas = difs[1].cantidad_minas;

                juego->mapa = (Casilla **)matrizCrear(juego->dificultad.dimension, juego->dificultad.dimension, sizeof(Casilla));
                juego->minasCoord = (Coord *)matrizCrear(juego->dificultad.cantidad_minas, 0, sizeof(Coord));
                *estado_actual = ESTADO_JUGANDO;
                graficos->tamXVentana = TAM_PIXEL * (difs[1].dimension * PIXELES_X_LADO + 20);
                graficos->tamYVentana = TAM_PIXEL * (difs[1].dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);
                graficos->anchoM = juego->dificultad.dimension * PIXELES_X_LADO + 4;
                SDL_SetWindowSize(ventana, graficos->tamXVentana, graficos->tamYVentana);
                // Logeo de seleccion
                setLog(&log, -1, -1, "Dificultad cargada: Intermedio.");
                mapaReiniciar(juego);
                escribirArchivoLog(juego->log, &log);
                break;

            case 2:
                // juego->dificultad.nombre = difs[2].nombre;
                juego->dificultad.dimension = difs[2].dimension;
                juego->dificultad.cantidad_minas = difs[2].cantidad_minas;

                juego->mapa = (Casilla **)matrizCrear(juego->dificultad.dimension, juego->dificultad.dimension, sizeof(Casilla));
                juego->minasCoord = (Coord *)matrizCrear(juego->dificultad.cantidad_minas, 0, sizeof(Coord));
                *estado_actual = ESTADO_JUGANDO;
                graficos->tamXVentana = TAM_PIXEL * (difs[2].dimension * PIXELES_X_LADO + 20);
                graficos->tamYVentana = TAM_PIXEL * (difs[2].dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);
                graficos->anchoM = juego->dificultad.dimension * PIXELES_X_LADO + 4;
                SDL_SetWindowSize(ventana, graficos->tamXVentana, graficos->tamYVentana);
                // Logeo de seleccion
                setLog(&log, -1, -1, "Dificultad cargada: Dificil.");
                mapaReiniciar(juego);
                escribirArchivoLog(juego->log, &log);
                break;
            }
            break;
        }
        break;
    }
    return 0;
}

int manejar_eventos_juego(Graficos *graficos, SDL_Event *e, EstadoJuego *estado_actual, EstadoJuego *estado_anterior, Juego *juego, Coord *picords, Coord *rbutton, Sonido *sonidos)
{

    if (estado_anterior == ESTADO_DIFICULTAD)
    {
        mapaReiniciar(juego);
    }
    Casilla **mapa = juego->mapa;

    int xG = ((e->button.x - (picords->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int yG = ((e->button.y - (picords->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int casillasLibresDeMinas = (juego->dificultad.dimension * juego->dificultad.dimension) - juego->dificultad.cantidad_minas;
    // guardado del boton anterior antes de nuevo evento
    int boton = e->button.button;
    int tecla = e->key.keysym.sym;

        // Para facilidad en correccion y lectura inicializo la var de los botones
    int botonAtras = graficos->tamXVentana-graficos->pad*10 + 5;
    int botonAdelante = graficos->tamXVentana-graficos->pad*6;
    int yBotones = graficos->piCord->y+graficos->altoC-graficos->G*2;

    if (e->type == SDL_MOUSEBUTTONDOWN)
    {
        juego->senialRender = 1;

        int boton = e->button.button; // guardado del boton anterior antes de nuevo evento
        EventoClick handlerClick;

        if ((rbutton->x * TAM_PIXEL <= e->button.x && e->button.x <= (rbutton->x + 28) * TAM_PIXEL) &&
            (rbutton->y * TAM_PIXEL <= e->button.y && e->button.y <= (rbutton->y + 28) * TAM_PIXEL) && (boton == SDL_BUTTON_LEFT))
        {
            Mix_PlayChannel(-1, sonidos->sonidoCat, 0);
            juego->iniciado = false;
            Log log;
            setLog(&log, -1, -1, "-Reinicio de juego-.");
            escribirArchivoLog(juego->log, &log);
        } else if (e->button.x >=  botonAtras && e->button.x <= botonAtras + 20 && // BOTON ATRAS
                    e->button.y >= yBotones && e->button.y <= yBotones + 20)
        {
            printf("aaa");
        }else if (e->button.x >=  botonAdelante && e->button.x <= botonAdelante + 20 && // BOTON ADELANTE
                    e->button.y >= yBotones && e->button.y <= yBotones + 20 )
        {
            printf("bbb");
        }
        else
        {
            // Control que no se toque fuera del mapa
            if (e->button.x > (graficos->tamXVentana - graficos->pad - 6 * graficos->G) || e->button.x <= (graficos->pad + 6 * graficos->G) ||
                e->button.y > (graficos->tamYVentana - graficos->pad - 6 * graficos->G) ||
                e->button.y <= (graficos->pad * 4 + graficos->altoC + 16 * graficos->G))
                return;

            int xG = ((e->button.x - (graficos->piCord->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
            int yG = ((e->button.y - (graficos->piCord->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
            int casillasLibresDeMinas = (juego->dificultad.dimension * juego->dificultad.dimension) - juego->dificultad.cantidad_minas;

            if (mapa[yG][xG].presionada && mapa[yG][xG].estado > 0)
            {
                handlerClick = clickDoble;
                handlerClick(juego, sonidos, xG, yG);
            }

            else
            {

                handlerClick = (boton == SDL_BUTTON_LEFT) ? handlerClickIzquierdo : handlerClickDerecho;
                handlerClick(juego, sonidos, xG, yG);

                if (juego->cantCasillasPresionadas == casillasLibresDeMinas)
                {
                    puts("¡Ganaste el juego!");
                    char *textoGanado[36];
                    sprintf(textoGanado, "Ganaste el juego! puntuacion: %d", juego->puntaje);
                    Log log;
                    setLog(&log, -1, -1, textoGanado);
                    escribirArchivoLog(juego->log, &log);
                    SDL_StartTextInput();
                    juego->nombreJugador[0] = '\0';
                    juego->finPartida = true;
                    juego->cantCasillasPresionadas++; // Para que no vuelva a entrar en la ventana de ganado
                    *estado_actual = ESTADO_GANADO;
                }
            }
        }
    }
    else if (e->type == SDL_KEYDOWN && tecla == SDLK_p)
    {
        *estado_actual = ESTADO_PAUSA;
    }
    return 0;
}

int manejar_eventos_ganado(SDL_Event *e, EstadoJuego *estado_actual, Juego *juego)
{

    int TAMX = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);

    // X e Y de boton cerrado
    int x = (TAMX / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12;
    int y = (TAMY / 2) - (TAMY_GANADO / 2) + 15 + 4;

    switch (e->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        juego->senialRender = 1;

        // Boton de cerrado ventana ganado
        if ((x <= e->button.x && e->button.x <= (x + 30)) &&
            (y <= e->button.y && e->button.y <= (y + 30)) &&
            (e->button.button == SDL_BUTTON_LEFT))
        {
            *estado_actual = ESTADO_JUGANDO;
        }
        break;
    case SDL_TEXTINPUT:
        juego->senialRender = 1;
        // Actualizacion de nombreJugador al presionar una tecla
        if (strlen(juego->nombreJugador) + strlen(e->text.text) <= 40)
        {
            strcat(juego->nombreJugador, e->text.text);
        }
        break;
    case SDL_KEYDOWN:
        juego->senialRender = 1;
        // Borrado de letra al presionar borrar
        if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(juego->nombreJugador) > 0)
            juego->nombreJugador[strlen(juego->nombreJugador) - 1] = '\0';
        // Guardado de puntaje al presionar Enter
        if (e->key.keysym.sym == SDLK_RETURN && strlen(juego->nombreJugador) > 0)
        {
            SDL_StopTextInput(); // Cierro la lectura de teclado

            // Abrir archivo
            FILE *aPuntuacionTemp = abrirArchivo("puntuacion.temp", "wt", juego);
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
            *estado_actual = ESTADO_JUGANDO;

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
    return 0;
}

//////////////////////////////////////////////////////////////////////

bool linea_ignorable(const char *linea)
{
    while (isspace(*linea))
        linea++;
    return (*linea == '\0' || *linea == '#');
}

int cargar_dificultades(const char *archivo, Dificultad *difs, int num_dif)
{

    FILE *config = fopen(archivo, "r");

    if (!config)
    {
        crear_dificultades_default(archivo);
        cargar_dificultades(archivo ,  difs , num_dif);
        return 0;
    }

    char linea[150];
    int i = 0;

    char minasTexto[5];

    while (fgets(linea, sizeof(linea), config) && i <= num_dif)
    {

        if (linea_ignorable(linea))
            continue;

        if (strncmp(linea, "DIFICULTAD", 10) == 0)
        {
            sscanf(linea, "DIFICULTAD = %20[^\n]", difs[i].nombre);

            printf("%s\n", difs[i].nombre);

            do
            {
                fgets(linea, sizeof(linea), config);
            } while (linea_ignorable(linea));

            sscanf(linea, "DIMENSION_MAPA = %i", &difs[i].dimension);

            if (difs[i].dimension < 8 || difs[i].dimension > 32)
            {
                puts("Error de configuracion DIMENSION_MAPA valores validos entre 8 y 32.");
                fclose(config);
                return ERROR_CONFIGURACION;
            }

            printf("%i\n", difs[i].dimension);

            do
            {
                fgets(linea, sizeof(linea), config);
            } while (linea_ignorable(linea));
            if (sscanf(linea, "CANTIDAD_MINAS = %5[^\n]", minasTexto) == -1)
            {
                puts("Error al leer CANTIDAD_MINAS.");
                fclose(config);
                return ERROR_ARCHIVO;
            }

            char *porcentaje = strchr(minasTexto, '%');
            difs[i].cantidad_minas = atoi(minasTexto);

            if (porcentaje)
                difs[i].cantidad_minas = round(((difs[i].dimension) * (difs[i].dimension)) * ((float)difs[i].cantidad_minas / 100));

            if (difs[i].cantidad_minas < 0)
            {
                puts("Error de configuracion CANTIDAD_MINAS valores negativos no validos");
                fclose(config);
                return ERROR_CONFIGURACION;
            }

            printf("%i\n\n", difs[i].cantidad_minas);

            i++;
        }
    }

    if (i != num_dif)
    {
        puts("Error al cargar todas las dificultades. Cerrando juego...");
        return ERROR_ARCHIVO;
    }

    fclose(config);
    return 0;
}

//////////////////////////////////////////////////////////////////////

int escribirArchivoLog(FILE *archivoLog, Log *log)
{
    if (log->coordXY[0] == -1 && log->coordXY[1] == -1)
    {
        fprintf(archivoLog, "[%d-%d-%d %02d:%02d:%02d] %-s\n",
                log->fechaHora.tm_year + 1900, log->fechaHora.tm_mon + 1, log->fechaHora.tm_mday,
                log->fechaHora.tm_hour, log->fechaHora.tm_min, log->fechaHora.tm_sec, log->tipoEvento);
    }
    else
    {
        fprintf(archivoLog, "[%d-%d-%d %02d:%02d:%02d] %-s | coordenadas: (%d , %d)\n",
                log->fechaHora.tm_year + 1900, log->fechaHora.tm_mon + 1, log->fechaHora.tm_mday,
                log->fechaHora.tm_hour, log->fechaHora.tm_min, log->fechaHora.tm_sec, log->tipoEvento,
                log->coordXY[0], log->coordXY[1]);
    }
    return 0;
}

int leerPuntajes(Juego *juego)
{
    // Abrir archivos
    FILE *aPuntuacion = abrirArchivo("puntuacion.txt", "rt", juego);
    if (!aPuntuacion)
    {
        // Si no existe lo creo
        FILE *aPuntuacion = abrirArchivo("puntuacion.txt", "wt", juego);
        if (!aPuntuacion)
        {
            return ERROR_ARCHIVO;
        }
        fclose(aPuntuacion); // Cierro el modo wt
    }

    // Dejo puntajes vacio para primera ejecucion
    if (!aPuntuacion)
    {
        juego->totalPuntajes = 0;
        return 0;
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

void crear_dificultades_default(const char* archivo){

    FILE* config = fopen(archivo, "w");

    if (!config){
        puts("Error al crear archivo de configuracion. Cerrando juego...");
        return ERROR_ARCHIVO;
    }

    fprintf(config , "#Configuracion de dificultad\n\n");

    fprintf(config , "DIFICULTAD = %s\n" ,       "Facil");
    fprintf(config , "DIMENSION_MAPA = %i\n" ,   8 );
    fprintf(config , "CANTIDAD_MINAS = %i\n\n" , 10);

    fprintf(config , "DIFICULTAD = %s\n" ,       "Intermedio");
    fprintf(config , "DIMENSION_MAPA = %i\n" ,   16);
    fprintf(config , "CANTIDAD_MINAS = %i\n\n" , 40);

    fprintf(config , "DIFICULTAD = %s\n" ,       "Dificil");
    fprintf(config , "DIMENSION_MAPA = %i\n" ,   32);
    fprintf(config , "CANTIDAD_MINAS = %i\n\n" , 99);

    fclose(config);
}