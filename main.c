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

bool linea_ignorable(const char* linea);
int cargar_dificultades(const char* archivo , Dificultad* difs , int num_dif);
int escribirArchivoLog(FILE* archivoLog, Log* log);
int leerPuntajes(Juego* juego);

int main(int argc, char *argv[]){

    //////////////////////////////////////////////////////////////////////

    // Iniciar SDL con funcion Video y audio
    if(SDL_Init(SDL_INIT_VIDEO) || SDL_Init(SDL_INIT_AUDIO))
    {
        puts("Error al iniciar SDL");
        return ERROR_CONFIGURACION;
    }

    Sonido sonidos;
    if(iniciarSonido(&sonidos))
    {
        return ERROR_CONFIGURACION;
    }

    //////////////////////////////////////////////////////////////////////

    // Tamaño de ancho y altura de la ventana, utilizo 1 sola variable ya que sera cuadrada
    int TAMX =  TAM_PIXEL * (16 * PIXELES_X_LADO + 20);
    int TAMY =  TAM_PIXEL * (16 * PIXELES_X_LADO + 4 + 3*8 + 28);
    // Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow("Buscaminas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    //////////////////////////////////////////////////////////////////////

    //Creacion archivo log
    FILE* archivoLog = fopen("partida.log", "w");

    if(!archivoLog){

        puts("Error creando el archivo log");
        return ERROR_ARCHIVO;
    }

    Log log;
    setLog(&log, -1, -1, "Inicio del juego");
    escribirArchivoLog(archivoLog, &log);

    //////////////////////////////////////////////////////////////////////

    // Inicio TTF y busco la fuente. Si no la encuentra imprime un error
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("Fonts/digital-7.ttf", 32);

    if (!font){

        printf("Error cargando fuente: %s\n", TTF_GetError());
        return ERROR_FUENTE;
    }

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

    Juego juego;
    juego.mapa = NULL;
    juego.iniciado = false;
    juego.senialRender = 1;

    // Inicializar el juego
    juego.puntaje = 0;
    juego.cantCasillasPresionadas = 0;
    //juego.cantMinasEnInterfaz = minasEnMapa;
    //juego.dimMapa = filas;
    juego.finPartida = false;
    juego.start_time = time(NULL); // Iniciar el contador cuando inicia el juego
    juego.nombreJugador[0] = '\0';

    // Leer archivo de puntajes
    leerPuntajes(&juego);

    //////////////////////////////////////////////////////////////////////

    if(cargarSonido("Sounds/sonidoMina.mp3", &sonidos.sonidoMina, 32)
        || cargarSonido("Sounds/sonidoCat.mp3", &sonidos.sonidoCat, 128)
        || cargarSonido("Sounds/sonidoClick.mp3", &sonidos.sonidoClick, 128)
        || cargarSonido("Sounds/sonidoBandera.mp3", &sonidos.sonidoBandera, 64)
        || cargarSonido("Sounds/sonidoPerdio.mp3", &sonidos.sonidoPerder, 32)
        || cargarSonido("Sounds/sonidoFlecha.mp3", &sonidos.sonidoFlecha, 32)
        || cargarSonido("Sounds/sonidoEnter.mp3", &sonidos.sonidoEnter, 32))
    {
        return ERROR_SONIDO;
    }

    if(cargarMusica("Sounds/musicaFondo.mp3", &sonidos.musicaFondo, 32)
        || cargarMusica("Sounds/musicaMenu.mp3", &sonidos.musicaMenu, 32))
    {
        return ERROR_SONIDO;
    }

    //////////////////////////////////////////////////////////////////////

    Coord picords = {0,0};
    Coord rbutton = {0,0};

    int corriendo = true; // Variable flag true para mantener corriendo el programa
    SDL_Event e; // Variable para registrar eventos
    EstadoJuego estado_actual = ESTADO_MENU; //Variable para estados
    iniciarMusicaMenu(&sonidos.musicaMenu);

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

    // While para mantener el programa corriendo
    while (corriendo){

        //////////////////////////////////////////////////////////////////////

        while(SDL_PollEvent(&e)){

            if(e.type == SDL_QUIT){

                corriendo = false;
                matrizDestruir(juego.mapa , juego.dificultad.dimension);
                printf("\n---Cerrando Ventana---\n");
            }

            switch(estado_actual){
                case ESTADO_MENU:
                    juego.senialRender=1;
                    manejar_eventos_menu(&e , &estado_actual , &seleccion_menu , menu_count , &sonidos);
                    break;

                case ESTADO_DIFICULTAD:
                    juego.senialRender=1;
                    manejar_eventos_dificultad(&graficos, &e , &estado_actual , &seleccion_dificultad , dificultad_count , &juego , dificultades , ventana);
                    break;

                case ESTADO_JUGANDO:
                    manejar_eventos_juego(&e , &estado_actual , &juego , &graficos , &rbutton , &sonidos);
                    break;

                case ESTADO_GANADO:
                    manejar_eventos_ganado(&e, &estado_actual, &juego);
                    break;

                case ESTADO_SALIENDO:
                    corriendo = false;
                    matrizDestruir(juego.mapa , juego.dificultad.dimension);
                    printf("\nSaliendo...\n");
                    break;
                default: break;
            }
        }

        //////////////////////////////////////////////////////////////////////
        if(!(estado_actual == ESTADO_GANADO) && (estado_actual==ESTADO_JUGANDO))
            tiempoYbombas(&graficos, &juego);

        //SDL_RenderClear(renderer);
        if(juego.senialRender){
            switch(estado_actual){

                case ESTADO_MENU:
                    dibujar_menu(&graficos, menu_items, menu_count, &seleccion_menu);
                    juego.senialRender=0;
                    break;

                case ESTADO_DIFICULTAD:
                    dibujar_menu(&graficos, dificultad_items , dificultad_count , &seleccion_dificultad);
                    juego.senialRender=0;
                    break;

                case ESTADO_JUGANDO:

                    if(!juego.iniciado){

                        mapaReiniciar(renderer, &juego);
                        system("cls");
                        graficos.anchoM = juego.dificultad.dimension * PIXELES_X_LADO + 4;
                        mapaImprimir(juego.mapa , juego.dificultad.dimension , juego.dificultad.dimension);

                    }
                    interfaz(&graficos, &juego, &rbutton);
                    casillaColocacion(renderer, juego.mapa , juego.dificultad.dimension , &picords);
                    juego.senialRender=0;
                    break;

                case ESTADO_GANADO:
                    interfazGanado(&graficos, &juego);
                    juego.senialRender=0;
                    break;
                default: break;
            }
        }
        //////////////////////////////////////////////////////////////////////

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    //////////////////////////////////////////////////////////////////////

    if(!juego.mapa) matrizDestruir(juego.mapa , juego.dificultad.dimension);

    fclose(archivoLog);
    FinalizarSDL(ventana, renderer, font, EXIT_SUCCESS, archivoLog); // Funcion para la finalizacion de SDL y sus componentes
    return EJECUCION_OK;
}

//////////////////////////////////////////////////////////////////////

int manejar_eventos_menu(SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int items_count , Sonido* sonidos){

    switch (e->type){

        case SDL_KEYDOWN:

            switch (e->key.keysym.sym){

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
                    switch (*seleccion){

                        case 0:
                            *estado_actual = ESTADO_DIFICULTAD;
                            iniciarMusicaJuego(&sonidos->musicaFondo);
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

int manejar_eventos_dificultad(Graficos *graficos, SDL_Event *e , EstadoJuego *estado_actual, int* seleccion , const int items_count , Juego* juego , Dificultad* difs , SDL_Window* ventana){

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

                            juego->mapa = (Casilla**)matrizCrear(juego->dificultad.dimension , juego->dificultad.dimension , sizeof(Casilla)); //Mover a otro lado despues
                            juego->minasCoord = (Coord *)matrizCrear(juego->dificultad.cantidad_minas , 0 , sizeof(Coord));
                            *estado_actual = ESTADO_JUGANDO;
                            graficos->tamXVentana = TAM_PIXEL * (difs[0].dimension * PIXELES_X_LADO + 20);
                            graficos->tamYVentana = TAM_PIXEL * (difs[0].dimension * PIXELES_X_LADO + 4 + 3*8 + 28);
                            SDL_SetWindowSize(ventana, graficos->tamXVentana, graficos->tamYVentana);
                            break;

                        case 1:
                            //juego->dificultad.nombre = difs[1].nombre;
                            juego->dificultad.dimension = difs[1].dimension;
                            juego->dificultad.cantidad_minas = difs[1].cantidad_minas;

                            juego->mapa = (Casilla**)matrizCrear(juego->dificultad.dimension , juego->dificultad.dimension , sizeof(Casilla));
                            juego->minasCoord = (Coord *)matrizCrear(juego->dificultad.cantidad_minas , 0 , sizeof(Coord));
                            *estado_actual = ESTADO_JUGANDO;
                            graficos->tamXVentana = TAM_PIXEL * (difs[1].dimension * PIXELES_X_LADO + 20);
                            graficos->tamYVentana = TAM_PIXEL * (difs[1].dimension * PIXELES_X_LADO + 4 + 3*8 + 28);
                            SDL_SetWindowSize(ventana, graficos->tamXVentana, graficos->tamYVentana);
                            break;

                        case 2:
                            //juego->dificultad.nombre = difs[2].nombre;
                            juego->dificultad.dimension = difs[2].dimension;
                            juego->dificultad.cantidad_minas = difs[2].cantidad_minas;

                            juego->mapa = (Casilla**)matrizCrear(juego->dificultad.dimension , juego->dificultad.dimension , sizeof(Casilla));
                            juego->minasCoord = (Coord *)matrizCrear(juego->dificultad.cantidad_minas , 0 , sizeof(Coord));
                            *estado_actual = ESTADO_JUGANDO;
                            graficos->tamXVentana = TAM_PIXEL * (difs[2].dimension * PIXELES_X_LADO + 20);
                            graficos->tamYVentana = TAM_PIXEL * (difs[2].dimension * PIXELES_X_LADO + 4 + 3*8 + 28);
                            SDL_SetWindowSize(ventana, graficos->tamXVentana, graficos->tamYVentana);
                            break;
                    }
                    break;
            }
            break;
    }
    return 0;
}

int manejar_eventos_juego(SDL_Event *e , EstadoJuego *estado_actual , Juego* juego , Graficos *graficos , Coord* rbutton , Sonido* sonidos){

    Casilla **mapa = juego->mapa;

    int xG = ((e->button.x - (graficos->piCord->x * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int yG = ((e->button.y - (graficos->piCord->y * TAM_PIXEL)) / (PIXELES_X_LADO * TAM_PIXEL));
    int casillasLibresDeMinas = (juego->dificultad.dimension * juego->dificultad.dimension) - juego->dificultad.cantidad_minas;

    if (e->type == SDL_MOUSEBUTTONDOWN){
        juego->senialRender=1;

        int boton = e->button.button; // guardado del boton anterior antes de nuevo evento
        EventoClick handlerClick;

        if ((rbutton->x * TAM_PIXEL <= e->button.x && e->button.x <= (rbutton->x + 28) * TAM_PIXEL) &&
            (rbutton->y * TAM_PIXEL <= e->button.y && e->button.y <= (rbutton->y + 28) * TAM_PIXEL) && (boton == SDL_BUTTON_LEFT))
        {
            Mix_PlayChannel(-1, sonidos->sonidoCat, 0);
            juego->iniciado = false;
        }

        else{

            if (mapa[yG][xG].presionada && mapa[yG][xG].estado > 0){

                handlerClick = clickDoble;
                Uint32 tiempoDeEspera = SDL_GetTicks() + 100;
                while (SDL_GetTicks() < tiempoDeEspera){

                    if (SDL_PollEvent(e) && e->type == SDL_MOUSEBUTTONDOWN && e->button.button != boton){

                        handlerClick(juego , sonidos , xG , yG);
                        continue;
                    }

                    SDL_Delay(1);
                }
            }

            else{

                handlerClick = (boton == SDL_BUTTON_LEFT) ? handlerClickIzquierdo : handlerClickDerecho;
                handlerClick(juego , sonidos , xG , yG);

                if (juego->cantCasillasPresionadas == casillasLibresDeMinas){

                    puts("¡Ganaste el juego!");
                    SDL_StartTextInput();
                    juego->nombreJugador[0] = '\0';
                    *estado_actual = ESTADO_GANADO;
                }
            }
        }
    }
    return 0;
}

int manejar_eventos_ganado(SDL_Event *e , EstadoJuego *estado_actual , Juego* juego){

    int TAMX = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 20);
    int TAMY = TAM_PIXEL * (juego->dificultad.dimension * PIXELES_X_LADO + 4 + 3 * 8 + 28);

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
                //rectanguloLlenoAbsoluto(renderer, RR, (win_width / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12, pcords->y + 15 + 4, TAM_BOTON_CERRADO, TAM_BOTON_CERRADO);
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
    return 0;
}

//////////////////////////////////////////////////////////////////////

bool linea_ignorable(const char* linea){
    while(isspace(*linea)) linea++;
    return (*linea == '\0' || *linea == '#');
}

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

//////////////////////////////////////////////////////////////////////

int escribirArchivoLog(FILE *archivoLog, Log *log){

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
