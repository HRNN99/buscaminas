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

    int filas = 0, columnas = 0, minasEnMapa = 0, guardado = 0;
    char rutaFuente[100];
    Puntaje puntaje[MAX_PUNTAJES];

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
    TTF_Font *font = TTF_OpenFont(rutaFuente, 24);
    if (!font)
    {
        printf("Error cargando fuente: %s\n", TTF_GetError());
        return ERROR_FUENTE;
    }

    char nombreJugador[40];
    char nombreVentana[100];
    // String formateado para el titulo de ventana
    sprintf(nombreVentana, "Buscaminas %ix%i", filas, columnas);
    // Tamaño de ancho y altura de la ventana, utilizo 1 sola variable ya que sera cuadrada
    int TAMX =  TAM_PIXEL * (columnas * PIXELES_X_LADO + 2*8 + 4);
    int TAMY =  TAM_PIXEL * (filas * PIXELES_X_LADO + 4 + 3*8 + 28);
    // Funcion para crear ventana con posicion especifica, dimension y banderas.
    SDL_Window *ventana = SDL_CreateWindow(nombreVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX, TAMY, 2);
    // Funcion para crear el renderizado en ventana acelerado por hardware
    SDL_Renderer *renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    // Funcion para establecer el modo de mezcla de colores para el renderizado, el modo blend nos permite utilizar transparencia
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Tamaño de ancho y altura de la ventana de ganado
    int TAMX_GANADO = TAMX-100;
    int TAMY_GANADO = TAMY-100;

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

    int boton, xGrilla, yGrilla, renderizarGanado = 0, fontSize = 16, casillasLibresDeMinas = (filas * columnas) - minasEnMapa;
    time_t current_time;
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

            renderizarTexto(font, fontSize, "Puntaje:", GF, GS, renderer, pad*3, pad+(altoC/2));
            char puntaje[21] = "";
            itoa(juego.puntaje, puntaje, 10); //Armado de String a imprimir
            renderizarTexto(font, fontSize, puntaje, GF, GS, renderer, pad*3, pad+(altoC/2)+fontSize+2);
            renderizarTexto(font, fontSize, "Minas:", GF, GS,renderer, (pad*3)+anchoM+22, pad+(altoC/2));

            char bombasEnMapaTexto[21] = "";
            itoa(juego.cantMinasEnInterfaz, bombasEnMapaTexto, 10); //Armado de String a imprimir
            renderizarTexto(font, fontSize, bombasEnMapaTexto, GF, GS, renderer, (pad*3)+anchoM+22, pad+(altoC/2)+fontSize+2);

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
            // Ventana
            rectanguloLleno(rendererGanado, GS, 0, 0, TAMX_GANADO, TAMY_GANADO);
            marcoInvertido(rendererGanado, 0, 0, (TAMX_GANADO)/2, (TAMY_GANADO)/2, 2);
            // Boton cerrar ventana
            rectanguloLlenoAbsoluto(rendererGanado, NN, TAMX_GANADO-15-20-6 , 15+4, 20, 20);
            marcoInvertido(rendererGanado, (TAMX_GANADO/2)-15-8, 15-8, 15, 15, 2);
            // Renderizar "Puntaje" y "Nombre:"
            char textoPuntaje[21] = "Puntaje: ";
            char puntajeChar[12];
            strcat(textoPuntaje, itoa(juego.puntaje, puntajeChar, 10)); //Armado de String a imprimir
            int posYtexto = 15;
            int margenX = 20;
            renderizarTexto(font, 30, "Ganaste!", BB, GS,rendererGanado, margenX, posYtexto);
            renderizarTexto(font, 24, textoPuntaje, BB, GS,rendererGanado, margenX, posYtexto+=45);
            renderizarTexto(font, 16, "Ingrese su nombre:", BB, GS, rendererGanado, margenX, posYtexto+=35);
            rectanguloLlenoAbsoluto(rendererGanado, BB, margenX, posYtexto+=40, 5,2); // Linea antes del nombre
            renderizarTexto(font, 20, nombreJugador, BB, GS,rendererGanado, 35, posYtexto-10); // Fix Y por como toma esa coordenada
            // Renderizar mejores posiciones
            renderizarTexto(font, 16, "Pepito 15", BB, GS,rendererGanado, margenX+20, posYtexto+=30);
            renderizarTexto(font, 16, "Juan 45", BB, GS,rendererGanado, margenX+20, posYtexto+=20);
            renderizarTexto(font, 16, "Rodo 126", BB, GS,rendererGanado, margenX+20, posYtexto+=20);
            // Mostrar todo
            SDL_RenderPresent(rendererGanado);
            renderizarGanado = 0;
            juego.finPartida = true;
        }

        if (SDL_PollEvent(&e))
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

                        printf("Reiniciaste mapa \n");
                        mapaReiniciar(renderer , &picords , &juego , filas , columnas , &minasCoord , minasEnMapa);
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
                            ventanaGanado = SDL_CreateWindow("Ganaste!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TAMX_GANADO, TAMY_GANADO, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
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
                    FILE* aPuntuacion = fopen("puntuacion.txt", "rt");
                    if(!aPuntuacion)
                    {
                        setLog(&log, -1, -1, "Error al abrir el archivo de puntuacion.");
                        escribirArchivoLog(archivoLog, &log);
                        puts("Error al abrir el archivo puntuacion.txt");
                        fclose(archivoLog);
                        return ERROR_ARCHIVO;
                    }
                    FILE* aPuntuacionTemp = fopen("puntuacion.temp", "wt");
                    if(!aPuntuacionTemp)
                    {
                        setLog(&log, -1, -1, "Error al abrir el archivo de puntuacionTemp.");
                        escribirArchivoLog(archivoLog, &log);
                        puts("Error al abrir el archivo puntuacion.temp");
                        fclose(archivoLog);
                        fclose(aPuntuacion);
                        return ERROR_ARCHIVO;
                    }
                    char linea[47];
                    char puntuacion[6];
                    int iterador = 0;
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
                        if (!guardado && juego.puntaje <= puntaje[i].puntos)
                        {
                            fprintf(aPuntuacionTemp, "%05d %-40s\n",  juego.puntaje,  nombreJugador);
                            guardado = 1;
                        }
                        fprintf(aPuntuacionTemp, "%05d %-40s\n",  puntaje[i].puntos,  puntaje[i].nombre);
                    }
                    guardado = 0;
                    fclose(aPuntuacion);
                    fclose(aPuntuacionTemp);

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
                    renderizarGanado = 0;
                    FinalizarVentanaSDL(ventanaGanado, rendererGanado); // Funcion para la finalizacion de SDL y sus componentes
               }
                break;
            }
        }

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
    printf("%d, %d, %s", *filas, *minasEnMapa, rutaFuente);
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
