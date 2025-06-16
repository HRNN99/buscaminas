#include <stdio.h>
#include "juego.h"
#include "estados.h"
#include "sonido.h"
#include "time.h"


void dibujar_menu(Graficos* graficos, const char *menu_items[], const int menu_count, int *seleccion)
{
    SDL_SetRenderDrawColor(graficos->renderer, 0, 0, 0, 255);
    SDL_RenderClear(graficos->renderer);

    int base_y = 50;
    int espacio = 50;

    for (int i = 0; i < menu_count; i++){

        int text_width, text_height;
        TTF_SizeText(graficos->font, menu_items[i], &text_width, &text_height);

        SDL_Rect fondo;
        fondo.x = (graficos->tamXVentana - text_width) / 2;
        fondo.y = base_y + i * espacio;
        fondo.w = text_width;
        fondo.h = text_height;

        // Establecemos un color para las letras del menu
        SDL_Color colorTexto = {255, 255, 255, 255};
        // Se establece un color de fondo, segun si el item esta seleccionado o no
        SDL_Color colorFondo = (i == *seleccion) ? (SDL_Color){255, 100, 255, 255} : (SDL_Color){100, 100, 100, 255};

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

    itoa(juego->dificultad.cantidad_minas, bombasEnMapaTexto, 10); // Armado de String a imprimir
    renderizarTexto(graficos->font, fontSize, bombasEnMapaTexto, GF, GS, graficos->renderer, (graficos->pad * 3) + graficos->anchoM + 22, graficos->pad + (graficos->altoC / 2) + fontSize + 4);

}

// TODO: agilizar esta funcion. Se ejeccuta todo le juego
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

void mapaReiniciar(SDL_Renderer *renderer, Juego *juego){

    juego->iniciado = true;
    Casilla **mapa = juego->mapa;

    juego->puntaje = 0;
    juego->finPartida = false;
    juego->cantCasillasPresionadas = 0;
    juego->start_time = time(NULL); // Iniciar el contador cuando inicia el juego
    juego->nombreJugador[0] = '\0';


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
        (juego->dificultad.cantidad_minas)--;
    else if (mapa[yG][xG].estadoBandera == 2)
        (juego->dificultad.cantidad_minas)++;

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

                if ((i == 0 && j == 0) || (gX + i < 0 || gX + i >= juego->dificultad.dimension || gY + j < 0 || gY + j >= juego->dificultad.dimension))
                    continue; // Evita repetirse a sí mismo
                if (gX < 0 || gX >= juego->dificultad.dimension || gY < 0 || gY >= juego->dificultad.dimension)
                    return;

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
