#include <stdio.h>
#include "juego.h"
#include "estados.h"
#include "sonido.h"
#include "time.h"


void dibujar_menu(Graficos* graficos, const char *menu_items[], const int menu_count, int *seleccion , SDL_Texture* fondo)
{
    SDL_SetWindowSize(graficos->ventana, TAM_PIXEL * (16 * PIXELES_X_LADO + 20),  TAM_PIXEL * (16 * PIXELES_X_LADO + 20));
    SDL_SetRenderDrawColor(graficos->renderer, 0, 0, 0, 255);
    SDL_RenderClear(graficos->renderer);

    dibujarFondo(graficos->renderer , fondo);

    int base_y = 50;
    int espacio = 50;

    for (int i = 0; i < menu_count; i++){

        int text_width, text_height;
        TTF_SizeText(graficos->font, menu_items[i], &text_width, &text_height);

        SDL_Rect fondo;
        fondo.x = (TAM_PIXEL * (16 * PIXELES_X_LADO + 20),  TAM_PIXEL * (16 * PIXELES_X_LADO + 20) - text_width) / 2;
        fondo.y = base_y + i * espacio;
        fondo.w = text_width;
        fondo.h = text_height;

        //Establecemos un color para las letras del menu
        SDL_Color colorTexto = colores[NN];

        SDL_Color colorFondo = {0,0,0,0}; //Color fondo default, transparente
        if(i == *seleccion)
            colorFondo = colores[RS]; //Color fondo seleccionado

        // Se crea una superficie de texto con el texto contenido en el menu[i]
        SDL_Surface *surface = TTF_RenderText_Solid(graficos->font, menu_items[i], colorTexto);
        // Convierte la superficie en una textura para poder ser renderizada
        SDL_Texture *texture = SDL_CreateTextureFromSurface(graficos->renderer, surface);

        // Se establece el color de dibujado usando color de fondo
        SDL_SetRenderDrawColor(graficos->renderer, colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a);
        // Se dibuja el rectangulo lleno en el render
        SDL_RenderFillRect(graficos->renderer, &fondo);

        // Calcula automaticamente el ancho y alto de texto renderizado, y actualiza valores de menu.rect
        SDL_QueryTexture(texture, NULL, NULL, &fondo.w, &fondo.h);
        // Se dibuja el texto en el render
        SDL_RenderCopy(graficos->renderer, texture, NULL, &fondo);

        // Se liberan variables temporales
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

// Funcion destinada a crear una matriz con memoria dinamica
Casilla **matrizCrear(size_t filas, size_t columnas, size_t tamElem){

    Casilla **mat = malloc(filas * sizeof(Casilla *));
    if (!mat)
    {

        puts("No hay suficiente memoria para las filas.");
        return NULL;
    }

    Casilla **ult = mat + filas - 1;

    for (Casilla **i = mat; i <= ult; i++)
    {

        *i = malloc(columnas * tamElem);

        if (!*i)
        {
            puts("No hay suficiente memoria para alguna de las filas.");
            matrizDestruir(mat, (i - mat));
            return NULL;
        }
    }

    return mat;
}

// Funcion para liberar la memoria de la matriz creada
void matrizDestruir(Casilla **mat, size_t filas){

    Casilla **ult = mat + filas - 1;

    for (Casilla **i = mat; i <= ult; i++)
    {
        free(*i);
    }

    free(mat);
}

// Funcion que inicializa el mapa de juego en valores por defecto
void mapaVacio(Casilla **mapa , int dimension){

    for (int y = 0; y < dimension; y++){

        for (int x = 0; x < dimension; x++){

            mapa[y][x].estado = 0;
            mapa[y][x].estadoBandera = 0;
            mapa[y][x].presionada = false;
        }
    }
}

// Funcion que llena el mapa de juego con minas y aleda�os
void mapaLlenar(Casilla **mapa , int dimension , Coord *minasCoord , int minas){

    int x, y, m = 0;
    srand(time(NULL));

    while (m < minas)
    {

        x = rand() % dimension;
        y = rand() % dimension;

        if (mapa[y][x].estado != -1)
        {

            minasCoord[m].x = x;
            minasCoord[m].y = y;

            mapa[y][x].estado = -1;

            // Sumar alrededro de la mina
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (dy == 0 && dx == 0)
                        continue;

                    int nf = y + dy;
                    int nc = x + dx;

                    if ((nf >= 0 && nf < dimension) && (nc >= 0 && nc < dimension) && (mapa[nf][nc].estado != -1))
                        mapa[nf][nc].estado++;
                }
            }

            m++;
        }
    }
}

void mapaImprimir(Casilla **mapa, int filas, int columnas){

    // TEST ONLY //Impresion de matriz
    for (size_t i = 0; i < filas; i++)
    {

        for (size_t j = 0; j < columnas; j++)
        {

            printf("%3d ", mapa[i][j].estado);
        }
        printf("\n\n");
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// Establecer el renderizado con un color base (negro)
void fondoColor(SDL_Renderer *renderer){

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // color
    SDL_RenderClear(renderer);                      // limpieza
    SDL_RenderPresent(renderer);                    // Aplicacion
}

void interfaz(Graficos *graficos, Juego *juego, Coord *rbutton)
{
    graficos->piCord->x = 0;
    graficos->piCord->y = 0;
    rbutton->x = 0;
    rbutton->y = 0;

    int anchoI = graficos->anchoM + 16;
    int altoI = graficos->pad*3 + graficos->altoC + graficos->anchoM;


    rectanguloLleno(graficos->renderer, GS, graficos->piCord->x, graficos->piCord->y, anchoI, altoI);
    marco(graficos->renderer, graficos->piCord->x, graficos->piCord->y, anchoI, altoI, graficos->G); // Exterior

    graficos->piCord->x += graficos->pad;
    graficos->piCord->y += graficos->pad;
    marco(graficos->renderer,graficos->piCord->x, graficos->piCord->y, graficos->anchoM, graficos->altoC, graficos->G); // Puntaje

    dibujar(graficos->renderer, PIXELES_X_LADO * 2, restart_button, 0, 0, (graficos->anchoM / 2) - 7, graficos->piCord->y);
    rbutton->x = ((graficos->anchoM / 2) - 7);
    rbutton->y = graficos->piCord->y;

    graficos->piCord->x += 0;
    graficos->piCord->y += graficos->altoC + graficos->pad;
    marco(graficos->renderer, graficos->piCord->x, graficos->piCord->y, graficos->anchoM, graficos->anchoM, graficos->G); // Mapa

    graficos->piCord->x += graficos->G;
    graficos->piCord->y += graficos->G;

    int fontSize = 16;
    renderizarTexto(graficos->font, fontSize, "Tiempo:", GF, GS, graficos->renderer, graficos->pad * 3, graficos->pad + (graficos->altoC / 2));
    renderizarTexto(graficos->font, fontSize, "Minas:", GF, GS, graficos->renderer, (graficos->pad * 3) + graficos->anchoM + 22, graficos->pad + (graficos->altoC / 2));
    char bombasEnMapaTexto[21] = "";

    itoa(juego->cantMinasEnMapa, bombasEnMapaTexto, 10); // Armado de String a imprimir
    renderizarTexto(graficos->font, fontSize, bombasEnMapaTexto, GF, GS, graficos->renderer, (graficos->pad * 3) + graficos->anchoM + 22, graficos->pad + (graficos->altoC / 2) + fontSize + 4);

}

void tiempoYbombas(Graficos *graficos, Juego *juego)
{

    // Renderizado de tiempo
    char puntaje[6] = "";
    itoa(juego->puntaje, puntaje, 10); // Armado de String a imprimir
    // Creo un rectangulo por detras de las letras para evitar limpiar el render
    SDL_Rect rectFondo = {graficos->pad * 3, graficos->pad + (graficos->altoC / 2) + graficos->fontSize + 4, 70, 16};
    SDL_SetRenderDrawColor(graficos->renderer , colores[GS].r , colores[GS].g , colores[GS].b , colores[GS].a);
    SDL_RenderFillRect(graficos->renderer , &rectFondo);
    renderizarTexto(graficos->font, graficos->fontSize, puntaje, GF, GS, graficos->renderer, graficos->pad * 3, graficos->pad + (graficos->altoC / 2) + graficos->fontSize + 4);

    // Aumento de puntaje por segundo
    if (!juego->finPartida)
    {
        time_t current_time = time(NULL);
        juego->puntaje = difftime(current_time, juego->start_time);
    }
}

void interfazGanado(Graficos *graficos, Juego *juego)
{
    // Ventana
    graficos->piCord->x = (graficos->tamXVentana / 2) - (TAMX_GANADO / 2);
    graficos->piCord->y = (graficos->tamYVentana / 2) - (TAMY_GANADO / 2);

    rectanguloLlenoAbsoluto(graficos->renderer, GS, graficos->piCord->x, graficos->piCord->y, TAMX_GANADO, TAMY_GANADO);
    marcoInvertido(graficos->renderer, graficos->piCord->x, graficos->piCord->y, (TAMX_GANADO), (TAMY_GANADO), 4);
    // Boton cerrar ventana

    dibujarAbsoluto(graficos->renderer, 20, close_button, (graficos->tamXVentana / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12 + 5, graficos->piCord->y + 15 + 4 + 5, 1);
    marcoInvertido(graficos->renderer, (graficos->tamXVentana / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12, graficos->piCord->y + 15 + 4, TAM_BOTON_CERRADO, TAM_BOTON_CERRADO, 4);

    char textoPuntaje[21] = "Tiempo: ";
    char puntajeChar[12];
    strcat(textoPuntaje, itoa(juego->puntaje, puntajeChar, 10)); // Armado de String a imprimir
    int posYtexto = graficos->piCord->y + 20;
    int margenX = graficos->piCord->x + 20;

    renderizarTexto(graficos->font, 30, "Ganaste!", BB, GS, graficos->renderer, margenX, posYtexto);
    renderizarTexto(graficos->font, 24, textoPuntaje, BB, GS, graficos->renderer, margenX, posYtexto += 45);

    renderizarTexto(graficos->font, 16, "Ingrese su nombre:", BB, GS, graficos->renderer, margenX, posYtexto += 35);
    rectanguloLlenoAbsoluto(graficos->renderer, BB, margenX, posYtexto += 40, 5, 2); // Linea antes del nombre
    char limitador[17] = {0}; // Limito los caracteres por temas visuales
    snprintf(limitador, 12, "%-s", juego->nombreJugador);

    renderizarTexto(graficos->font, 20, juego->nombreJugador, BB, GS, graficos->renderer, margenX + 15, posYtexto - 12); // Fix Y por como toma esa coordenada

    // Renderizar mejores posiciones
    if(juego->totalPuntajes >= 3){
        dibujarAbsoluto(graficos->renderer, 24, construirCoronaConColores(corona, GS, AD, DS), margenX, posYtexto+=30, 1);
        snprintf(limitador, sizeof(limitador), "%05d %-s", juego->puntajes[0].puntos, juego->puntajes[0].nombre);
        renderizarTexto(graficos->font, 16, limitador, BB, GS, graficos->renderer, margenX + 35, posYtexto + 5);

        dibujarAbsoluto(graficos->renderer, 24, construirCoronaConColores(corona, GS, BB, GA), margenX, posYtexto+=25, 1);
        snprintf(limitador, sizeof(limitador), "%05d %-s", juego->puntajes[1].puntos, juego->puntajes[1].nombre);
        renderizarTexto(graficos->font, 16, limitador, BB, GS, graficos->renderer, margenX + 35, posYtexto + 5);

        snprintf(limitador, sizeof(limitador), "%05d %-s", juego->puntajes[2].puntos, juego->puntajes[2].nombre);
        dibujarAbsoluto(graficos->renderer, 24, construirCoronaConColores(corona, GS, BR, BS), margenX, posYtexto+=25, 1);
        renderizarTexto(graficos->font, 16, limitador, BB, GS, graficos->renderer, margenX + 35, posYtexto + 5);
    }
    // Mostrar todo
    SDL_RenderPresent(graficos->renderer);
    juego->finPartida = true;
}


void interfazPausa(SDL_Renderer *renderer, SDL_Window *ventana, TTF_Font *font, Juego *juego, Coord *pcords, int dimensionM, Coord *rbutton, int *eleccion, bool musicaActiva, const char *menu_items[], int menu_count)
{


    int win_width, win_height;
    SDL_GetWindowSize(ventana, &win_width, &win_height);


    pcords->x = (win_width / 2) - (TAMX_GANADO / 2);
    pcords->y = (win_height / 2) - (TAMY_GANADO / 2);
    rectanguloLlenoAbsoluto(renderer, GS, pcords->x, pcords->y, TAMX_GANADO, TAMY_GANADO);
    marcoInvertido(renderer, pcords->x, pcords->y, TAMX_GANADO, TAMY_GANADO, 4);

    // Boton de pausar musica
    dibujarAbsoluto(renderer, 20, 
                    musicaActiva ? music_button : music_button_pausa,
                    (win_width / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12 + 5, pcords->y + 15 + 4 + 5, 1);

    marcoInvertido(renderer, (win_width / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12, pcords->y + 15 + 4, TAM_BOTON_CERRADO, TAM_BOTON_CERRADO, 4);
    marcoInvertido(renderer, (win_width / 2) + (TAMX_GANADO / 2) - 15 - 20 - 12, pcords->y + 15 + 4, TAM_BOTON_CERRADO, TAM_BOTON_CERRADO, 4);

    // Texto principal y puntaje
    char textoPuntaje[21] = "Tiempo: ";
    char puntajeChar[12];
    strcat(textoPuntaje, itoa(juego->puntaje, puntajeChar, 10));
    int posYtexto = pcords->y + 20;
    int margenX = pcords->x + 20;

    renderizarTexto(font, 30, "Pausa", BB, GS, renderer, margenX, posYtexto);
    renderizarTexto(font, 24, textoPuntaje, BB, GS, renderer, margenX, posYtexto += 45);

    // Menú de opciones
    int espacio = 45;
    posYtexto += 35;

    for (int i = 0; i < menu_count; i++) {
        SDL_Color colorTexto = {255, 255, 255, 255};
        SDL_Color colorFondo = (i == *eleccion) ? (SDL_Color){255, 100, 255, 255} : (SDL_Color){100, 100, 100, 255};

        SDL_Surface *surface = TTF_RenderText_Solid(font, menu_items[i], colorTexto);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        int text_width, text_height;
        TTF_SizeText(font, menu_items[i], &text_width, &text_height);
        SDL_Rect fondo = {margenX, posYtexto, TAMX_GANADO - 40, text_height + 10};

        SDL_SetRenderDrawColor(renderer, colorFondo.r, colorFondo.g, colorFondo.b, colorFondo.a);
        SDL_RenderFillRect(renderer, &fondo);

        SDL_Rect textRect = {
            fondo.x + (fondo.w - text_width) / 2,
            fondo.y + (fondo.h - text_height) / 2,
            text_width,
            text_height
        };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        posYtexto += espacio;
    }


}

void mapaReiniciar(Juego *juego){

    juego->iniciado = true;
    Casilla **mapa = juego->mapa;

    juego->puntaje = 0;
    juego->finPartida = false;
    juego->cantCasillasPresionadas = 0;
    juego->start_time = time(NULL); // Iniciar el contador cuando inicia el juego
    juego->nombreJugador[0] = '\0';
    juego->cantMinasEnMapa = juego->dificultad.cantidad_minas;
    mapaVacio(mapa , juego->dificultad.dimension);
    mapaLlenar(mapa , juego->dificultad.dimension , juego->minasCoord , juego->dificultad.cantidad_minas);
}

// Funcion que coloca todas las casillas sin valor
void casillaColocacion(SDL_Renderer *renderer , Casilla **mapa , int dimension , Coord *picord){


    int gX = 0; // Variable para coordenada X
    int gY = 0; // Variable para ccoordenada Y

    int x, y = 0;

    while (y < dimension){

        x = 0;
        while (x < dimension){

            //mapa[y][x].estadoBandera = 0; // Uso el ciclo para inicializar todo en 0
            gX = x % dimension;
            gY = y % dimension;

            // Si la casilla no esta presionada dibujo el estado por defecto
            if (!mapa[y][x].presionada){

                if (mapa[y][x].estadoBandera != 0)
                {
                    dibujar(renderer, PIXELES_X_LADO, eleccionBandera(mapa[gY][gX].estadoBandera), gX, gY, picord->x, picord->y);
                }
                else
                {
                    dibujar(renderer, PIXELES_X_LADO, square1 , gX, gY, picord->x, picord->y);
                }

            }
            // Si la casilla esta presionada dibujo su valor de estado
            else{

                dibujar(renderer, PIXELES_X_LADO, eleccion(mapa[y][x].estado), gX, gY, picord->x, picord->y);
            }

            // if(mapa[y][x].estadoBandera >= 0){
            //
            // }

            x++;
        }
        y++;
    }
}

// Funcion que coloca estados en las casillas
void casillaEstado(Juego *juego , Sonido *sonidos , int gX, int gY, bool chordClick){

    if (gX < 0 || gX >= juego->dificultad.dimension || gY < 0 || gY >= juego->dificultad.dimension)
        return;

    Casilla *casillaSeleccionada = &juego->mapa[gY][gX];
    Casilla *casillaBandera = &juego->mapa[gY][gX];

    if (chordClick && casillaSeleccionada->presionada && casillaSeleccionada->estado > 0){

        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                if (i == 0 && j == 0)
                    continue; // Evita repetirse a sí mismo
                casillaEstado(juego , sonidos , gX + i , gY + j , false);
            }
        }
    }

    // No hacer nada si ya está presionada o tiene bandera
    if (casillaSeleccionada->presionada || casillaBandera->estadoBandera != 0)
        return;

    casillaSeleccionada->presionada = true;
    juego->cantCasillasPresionadas++;

    // Juego Perdido
    if (casillaSeleccionada->estado == -1){

        Mix_PauseMusic();

        juego->finPartida = true;


        // Mostrar todas las bombas
        for (int i = 0; i < juego->dificultad.cantidad_minas ; i++){

            int mX = juego->minasCoord[i].x;
            int mY = juego->minasCoord[i].y;

            if (gX == mX && gY == mY)
                continue;
            juego->mapa[mY][mX].presionada = true; // Presionar todas las minas
            juego->mapa[mY][mX].estado = -2;        // Estado de mina revelada
        }

        Mix_PlayChannel(-1, sonidos->sonidoPerder, 0); // Sonido de mina
        SDL_Delay(2000); // Espera para reproducir el sonido
        Mix_PlayMusic(sonidos->musicaFondo, -1); // Musica de menu
        return;
    }

    // Dibuja numero y termina
    if (casillaSeleccionada->estado > 0 || casillaSeleccionada->estado == -1){

        // dibujar(renderer, PIXELES_X_LADO, eleccion(casillaSeleccionada->estado), gX, gY, picords->x, picords->y);
        return;
    }
    // dibujar(renderer, PIXELES_X_LADO, square2, gX, gY, picords->x, picords->y);

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {

            if (i == 0 && j == 0)
                continue; // Evita repetirse a sí mismo
            casillaEstado(juego , sonidos , gX + i , gY + j , false);

        }
    }
}

// Funcion para colocar bandera
void casillaBandera(Juego *juego, int xG, int yG){

    if (xG < 0 || xG >= juego->dificultad.dimension || yG < 0 || yG >= juego->dificultad.dimension)
        return;

    Casilla **mapa = juego->mapa;
    // Realizo una iteracion ciclica con el resto
    //  1%3 = 1, 2%3 = 2, 3%3 = 0;
    mapa[yG][xG].estadoBandera = (mapa[yG][xG].estadoBandera + 1) % 3;

    // Suma y resta de bombas dependiendo el caso
    if (mapa[yG][xG].estadoBandera == 1)
        (juego->cantMinasEnMapa)--;
    else if (mapa[yG][xG].estadoBandera == 2)
        (juego->cantMinasEnMapa)++;

    // dibujar(renderer, PIXELES_X_LADO, eleccionBandera(mapa[gY][gX].estadoBandera), gX, gY, picord->x, picord->y);
    printf("Estado (%i , %i): %i\n", xG, yG, mapa[yG][xG].estadoBandera);
}

// funciones Log
void setLog(Log *log, int coordX, int coordY, char tipoEvento[80]){

    time_t ahora = time(NULL);
    struct tm *aux = localtime(&ahora);
    log->fechaHora = *aux; // Asignar la fecha y hora actual al log
    strcpy(log->tipoEvento, tipoEvento);
    log->coordXY[0] = coordX;
    log->coordXY[1] = coordY;
}

// Función para abrir un archivo y manejar errores
FILE *abrirArchivo(const char *nombre, const char *modo){

    FILE *archivo = fopen(nombre, modo);
    if (!archivo)
    {
        // setLog(&log, -1, -1, "Error al abrir el archivo.");
        // escribirArchivoLog(archivoLog, &log);
        puts("Error al abrir el archivo.");
        return NULL;
    }
    return archivo;
}

void clickDoble(Juego *juego , Sonido *sonidos , int gX , int gY){

    Casilla **mapa = juego->mapa;

    int cont = 0;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (mapa[gY + j][gX + i].estadoBandera == 1)
            {

                int nx = gX + i;
                int ny = gY + j;
                if (i == 0 && j == 0)
                    continue;
                if (nx < 0 || nx >= juego->dificultad.dimension || ny < 0 || ny >= juego->dificultad.dimension)
                    continue;
                if (mapa[ny][nx].estadoBandera == 1)
                    cont++;
            }
        }
    }

    if (mapa[gY][gX].estado == cont)
    {

        casillaEstado(juego , sonidos , gX , gY , true);
    }
    return;
}

// clickHandlers
void handlerClickIzquierdo(Juego *juego , Sonido *sonidos , int x , int y){

    printf("Hiciste click en la casilla (%i , %i)\n", x, y);
    if(!juego->finPartida)
        casillaEstado(juego , sonidos , x , y , false);
    if(juego->mapa[y][x].estado == -1)
         Mix_PlayChannel(-1, sonidos->sonidoMina, 0);
    else if(juego->mapa[y][x].estadoBandera == 0){
        Mix_PlayChannel(-1, sonidos->sonidoClick, 0); // Reproduce sonido de mina si se presiona una mina
    }
}

void handlerClickDerecho(Juego *juego , Sonido *sonidos , int x , int y){

    printf("Hiciste click derecho en la casilla (%i , %i), colocando bandera\n", x, y);
    if(!juego->finPartida)
        casillaBandera(juego , x , y);
    if(juego->mapa[y][x].presionada == 0)
        Mix_PlayChannel(-1, sonidos->sonidoBandera, 0);
}


//CARGADO DE PARTIDAS

void convertirAJuegoGuardado(Juego *origen, JuegoGuardado *destino) {
    destino->iniciado = origen->iniciado;
    destino->cantCasillasPresionadas = origen->cantCasillasPresionadas;
    destino->puntaje = origen->puntaje;
    destino->dificultad.cantidad_minas = origen->dificultad.cantidad_minas;
    destino->dificultad.dimension = origen->dificultad.dimension;
    strncpy(destino->nombreJugador, origen->nombreJugador, 40);
    destino->finPartida = origen->finPartida;
    destino->totalPuntajes = origen->totalPuntajes;
    destino->start_time = origen->start_time;
    memcpy(destino->puntajes, origen->puntajes, sizeof(Puntaje) * MAX_PUNTAJES);

    int dim = origen->dificultad.dimension;
    for (int i = 0; i < dim; ++i)
        for (int j = 0; j < dim; ++j)
            destino->mapa[i * dim + j] = origen->mapa[i][j];
}

void convertirAJuego(JuegoGuardado *origen, Juego *destino) {
    destino->iniciado = origen->iniciado;
    destino->cantCasillasPresionadas = origen->cantCasillasPresionadas;
    destino->puntaje = origen->puntaje;
    destino->dificultad.cantidad_minas = origen->dificultad.cantidad_minas;
    destino->dificultad.dimension = origen->dificultad.dimension;
    strncpy(destino->nombreJugador, origen->nombreJugador, 40);
    destino->finPartida = origen->finPartida;
    destino->totalPuntajes = origen->totalPuntajes;
    destino->start_time = origen->start_time;
    memcpy(destino->puntajes, origen->puntajes, sizeof(Puntaje) * MAX_PUNTAJES);

    int dim = origen->dificultad.dimension;
    destino->mapa = malloc(dim * sizeof(Casilla *));
    for (int i = 0; i < dim; ++i) {
        destino->mapa[i] = malloc(dim * sizeof(Casilla));
        for (int j = 0; j < dim; ++j)
            destino->mapa[i][j] = origen->mapa[i * dim + j];
    }
}


bool archivoExiste(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

void inicializarPartidas(Juego partidas[3]) {
    for (int i = 0; i < 3; ++i) {
        partidas[i].iniciado = false;
        partidas[i].finPartida = false;
        partidas[i].cantCasillasPresionadas = 0;
        partidas[i].puntaje = 0;
        partidas[i].dificultad.dimension = 10;
        strcpy(partidas[i].nombreJugador, "Sin nombre");
        partidas[i].totalPuntajes = 0;
        partidas[i].start_time = time(NULL);

        int dim = partidas[i].dificultad.dimension;
        partidas[i].mapa = malloc(dim * sizeof(Casilla *));
        for (int j = 0; j < dim; ++j) {
            partidas[i].mapa[j] = malloc(dim * sizeof(Casilla));
            for (int k = 0; k < dim; ++k) {
                partidas[i].mapa[j][k].estado = 0;
                partidas[i].mapa[j][k].presionada = false;
                partidas[i].mapa[j][k].estadoBandera = 0;
            }
        }
    }
}


void guardarEnSlot(Juego *juego, int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) 
        return;

    Juego juegoAux[3];
    cargarPartidas(juegoAux, ARCHIVO_PARTIDAS); // cargar todos los slots actuales

    // Copiar campos base
    juegoAux[slot].iniciado = juego->iniciado;
    juegoAux[slot].cantCasillasPresionadas = juego->cantCasillasPresionadas;
    juegoAux[slot].puntaje = juego->puntaje;
    juegoAux[slot].dificultad.dimension = juego->dificultad.dimension;
    juegoAux[slot].dificultad.dimension = juego->dificultad.dimension;
    juegoAux[slot].finPartida = juego->finPartida;
    juegoAux[slot].start_time = juego->start_time;
    juegoAux[slot].totalPuntajes = juego->totalPuntajes;
    memcpy(juegoAux[slot].nombreJugador, juego->nombreJugador, sizeof(juego->nombreJugador));
    memcpy(juegoAux[slot].puntajes, juego->puntajes, sizeof(juego->puntajes));


    int dim = juego->dificultad.dimension;
    juegoAux[slot].mapa = malloc(dim * sizeof(Casilla *));
    for (int i = 0; i < dim; i++) {
        juegoAux[slot].mapa[i] = malloc(dim * sizeof(Casilla));
        for (int j = 0; j < dim; j++) {
            juegoAux[slot].mapa[i][j] = juego->mapa[i][j];
        }
    }

    guardarPartidas(juegoAux, ARCHIVO_PARTIDAS);
}

int cargarDesdeSlot(Graficos *graficos,Juego *juego, int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) 
        return 1;

    Juego juegoAux[3];
    if(cargarPartidas(juegoAux, ARCHIVO_PARTIDAS)) return 1;

    // Liberar mapa previo si existe
    if (juego->mapa != NULL) {
        matrizDestruir(juego->mapa, juego->dificultad.dimension);
    }

    // Copiar campos base
    juego->iniciado = juegoAux[slot].iniciado;
    juego->cantCasillasPresionadas = juegoAux[slot].cantCasillasPresionadas;
    juego->puntaje = juegoAux[slot].puntaje;
    juego->dificultad.dimension = juegoAux[slot].dificultad.dimension;
    juego->finPartida = juegoAux[slot].finPartida;
    juego->start_time = juegoAux[slot].start_time;
    juego->totalPuntajes = juegoAux[slot].totalPuntajes;
    memcpy(juego->nombreJugador, juegoAux[slot].nombreJugador, sizeof(juego->nombreJugador));
    memcpy(juego->puntajes, juegoAux[slot].puntajes, sizeof(juego->puntajes));

    int dim = juegoAux[slot].dificultad.dimension;
    juego->mapa = malloc(dim * sizeof(Casilla *));
    for (int i = 0; i < dim; i++) {
        juego->mapa[i] = malloc(dim * sizeof(Casilla));
        for (int j = 0; j < dim; j++) {
            juego->mapa[i][j] = juegoAux[slot].mapa[i][j];
        }
    }
    return 0;
    
}

void guardarPartidas(Juego partidas[3], const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) return;
    JuegoGuardado aux[3];
    for (int i = 0; i < 3; ++i)
        convertirAJuegoGuardado(&partidas[i], &aux[i]);
    fwrite(aux, sizeof(JuegoGuardado), 3, file);
    fclose(file);
}

int cargarPartidas(Juego partidas[3], const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return 1;
    JuegoGuardado aux[3];
    fread(aux, sizeof(JuegoGuardado), 3, file);
    for (int i = 0; i < 3; ++i)
        convertirAJuego(&aux[i], &partidas[i]);
    fclose(file);
    return 0;
}