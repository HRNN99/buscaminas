#include <stdio.h>
#include "juego.h"
#include "estados.h"
#include "time.h"

void dibujar_menu(SDL_Renderer* renderer , SDL_Window* ventana , TTF_Font* font , const char* menu_items[] , const int menu_count , int* seleccion){

    SDL_SetRenderDrawColor(renderer , 0 , 0 , 0 , 255);
    SDL_RenderClear(renderer);

    int win_width, win_height;
    SDL_GetWindowSize(ventana , &win_width , &win_height);

    int base_y = 50;
    int espacio = 50;

    for(int i = 0 ; i < menu_count ; i++){

        int text_width, text_height;
        TTF_SizeText(font , menu_items[i] , &text_width , &text_height);

        SDL_Rect fondo;
        fondo.x = (win_width - text_width) / 2;
        fondo.y = base_y + i*espacio;
        fondo.w = text_width;
        fondo.h = text_height;

        //Establecemos un color para las letras del menu
        SDL_Color colorTexto = {255,255,255,255};
        //Se establece un color de fondo, segun si el item esta seleccionado o no
        SDL_Color colorFondo = (i == *seleccion) ? (SDL_Color){255,100,255,255} : (SDL_Color){100,100,100,255};

        //Se crea una superficie de texto con el texto contenido en el menu[i]
        SDL_Surface *surface = TTF_RenderText_Solid(font , menu_items[i] , colorTexto);
        //Convierte la superficie en una textura para poder ser renderizada
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer , surface);

        //Se establece el color de dibujado usando color de fondo
        SDL_SetRenderDrawColor(renderer , colorFondo.r , colorFondo.g , colorFondo.b , colorFondo.a);
        //Se dibuja el rectangulo lleno en el render
        SDL_RenderFillRect(renderer , &fondo);

        //Calcula automaticamente el ancho y alto de texto renderizado, y actualiza valores de menu.rect
        SDL_QueryTexture(texture , NULL , NULL ,&fondo.w , &fondo.h);
        //Se dibuja el texto en el render
        SDL_RenderCopy(renderer , texture , NULL , &fondo);

        //Se liberan variables temporales
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

    }
}

// Funcion destinada a crear una matriz con memoria dinamica
Casilla **matrizCrear(size_t filas, size_t columnas, size_t tamElem)
{

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
void matrizDestruir(Casilla **mat, size_t filas)
{

    Casilla **ult = mat + filas - 1;

    for (Casilla **i = mat; i <= ult; i++)
    {
        free(*i);
    }

    free(mat);
}

// Funcion que inicializa el mapa de juego en valores por defecto
void mapaVacio(Casilla **mapa, int filas, int columnas)
{

    for (int y = 0; y < filas; y++)
    {
        for (int x = 0; x < columnas; x++)
        {
            mapa[y][x].estado = 0;
            mapa[y][x].estadoBandera = 0;
            mapa[y][x].presionada = false;
        }
    }
}

// Funcion que llena el mapa de juego con minas y aleda�os
void mapaLlenar(Casilla **mapa, int filas, int columnas, Coord *minasCoord, int minas)
{

    int x, y, m = 0;
    srand(time(NULL));

    while (m < minas)
    {

        x = rand() % columnas;
        y = rand() % filas;

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

                    if ((nf >= 0 && nf < filas) && (nc >= 0 && nc < columnas) && (mapa[nf][nc].estado != -1))
                        mapa[nf][nc].estado++;
                }
            }

            m++;
        }
    }
}

void mapaImprimir(Casilla** mapa, int filas, int columnas){

    // TEST ONLY //Impresion de matriz
    for (size_t i = 0; i < filas; i++){

        for (size_t j = 0; j < columnas; j++){

            printf("%3d ", mapa[i][j].estado);
        }
        printf("\n\n");
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// Establecer el renderizado con un color base (negro)
void fondoColor(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // color
    SDL_RenderClear(renderer);                      // limpieza
    SDL_RenderPresent(renderer);                    // Aplicacion
}

void interfaz(SDL_Renderer *renderer, Coord *pcords, int dimensionM, Coord *rbutton)
{
    pcords->x = 0;
    pcords->y = 0;
    rbutton->x = 0;
    rbutton->y = 0;

    int G = 2; // Grosor
    int pad = G * 4;

    int anchoM = dimensionM * PIXELES_X_LADO + 4;
    int altoC = 28;
    int anchoI = anchoM + 16;
    int altoI = pad + altoC + pad + anchoM + pad;

    rectanguloLleno(renderer, GS, pcords->x, pcords->y, anchoI, altoI);

    marco(renderer, pcords->x, pcords->y, anchoI, altoI, G); // Exterior

    pcords->x += pad;
    pcords->y += pad;
    marco(renderer, pcords->x, pcords->y, anchoM, altoC, G); // Puntaje

    dibujar(renderer, PIXELES_X_LADO * 2, restart_button, 0, 0, (anchoM / 2) - 7, pcords->y);
    rbutton->x = ((anchoM / 2) - 7);
    rbutton->y = pcords->y;

    pcords->x += 0;
    pcords->y += altoC + pad;
    marco(renderer, pcords->x, pcords->y, anchoM, anchoM, G); // Mapa

    pcords->x += G;
    pcords->y += G;
}

void interfazGanado(SDL_Renderer *renderer, SDL_Window* ventana, TTF_Font* font, Juego* juego, Coord *pcords, int dimensionM, Coord *rbutton)
{
    int win_width, win_height;
    SDL_GetWindowSize(ventana , &win_width , &win_height);

    // Ventana
    int TAMX_GANADO = 220, TAMY_GANADO = 276, TAM_BOTON = 30;
    pcords->x = (win_width/2)-(TAMX_GANADO/2);
    pcords->y = (win_height/2)-(TAMY_GANADO/2);
    rectanguloLlenoAbsoluto(renderer, GS, pcords->x, pcords->y, TAMX_GANADO, TAMY_GANADO);
    marcoInvertido(renderer, pcords->x, pcords->y, (TAMX_GANADO), (TAMY_GANADO), 4);
    // Boton cerrar ventana
    rectanguloLlenoAbsoluto(renderer, RR, (win_width/2)+(TAMX_GANADO/2)-15-20-12 , pcords->y+15+4, TAM_BOTON, TAM_BOTON);
    marcoInvertido(renderer, (win_width/2)+(TAMX_GANADO/2)-15-20-12 , pcords->y+15+4, TAM_BOTON, TAM_BOTON, 4);
    // Renderizar "Puntaje" y "Nombre:"
    char textoPuntaje[21] = "Puntaje: ";
    char puntajeChar[12];
    strcat(textoPuntaje, itoa(juego->puntaje, puntajeChar, 10)); //Armado de String a imprimir
    int posYtexto = pcords->y + 20;
    int margenX = pcords->x + 20;
    renderizarTexto(font, 30, "Ganaste!", BB, GS,renderer, margenX, posYtexto);
    renderizarTexto(font, 24, textoPuntaje, BB, GS,renderer, margenX, posYtexto+=45);
    renderizarTexto(font, 16, "Ingrese su nombre:", BB, GS, renderer, margenX, posYtexto+=35);
    rectanguloLlenoAbsoluto(renderer, BB, margenX, posYtexto+=40, 5,2); // Linea antes del nombre
    renderizarTexto(font, 20, juego->nombreJugador, BB, GS,renderer, margenX + 15, posYtexto-12); // Fix Y por como toma esa coordenada
    // Renderizar mejores posiciones
    renderizarTexto(font, 16, "Pepito 15", BB, GS,renderer, margenX+20, posYtexto+=30);
    renderizarTexto(font, 16, "Juan 45", BB, GS,renderer, margenX+20, posYtexto+=20);
    renderizarTexto(font, 16, "Rodo 126", BB, GS,renderer, margenX+20, posYtexto+=20);
    // Mostrar todo
    SDL_RenderPresent(renderer);
    juego->finPartida = true;
}

void mapaReiniciar(SDL_Renderer *renderer, Coord *pcord, Juego *juego, int filas, int columnas, Coord *minasCoord, int minas){

    juego->iniciado = true;
    Casilla **mapa = juego->mapa;

    juego->puntaje = 0;
    juego->dimMapa = filas;
    juego->cantMinasEnInterfaz = minas;
    juego->finPartida = false;
    juego->cantCasillasPresionadas = 0;
    juego->start_time = time(NULL); // Iniciar el contador cuando inicia el juego
    juego->nombreJugador[0] = '\0';
    
    mapaVacio(mapa, filas, columnas);
    mapaLlenar(mapa, filas, columnas, minasCoord, minas);
}

// Funcion que coloca todas las casillas sin valor
void casillaColocacion(Casilla **mapa, SDL_Renderer *renderer, int fil, int col, Coord *picord){

    int gX = 0; // Variable para coordenada X
    int gY = 0; // Variable para ccoordenada Y

    int x, y = 0;
    while (y < fil){

        x = 0;
        while (x < col){

            //mapa[y][x].estadoBandera = 0; // Uso el ciclo para inicializar todo en 0
            gX = x % col;
            gY = y % fil;


            //Si la casilla no esta presionada dibujo el estado por defecto
            if(!mapa[y][x].presionada){

                if(mapa[y][x].estadoBandera != 0){
                    dibujar(renderer, PIXELES_X_LADO, eleccionBandera(mapa[gY][gX].estadoBandera), gX, gY, picord->x, picord->y);
                }
                else{
                    dibujar(renderer, PIXELES_X_LADO, square1 , gX , gY , picord->x, picord->y);
                }

            }
            //Si la casilla esta presionada dibujo su valor de estado
            else{
                dibujar(renderer, PIXELES_X_LADO, eleccion(mapa[y][x].estado) , gX , gY , picord->x, picord->y);
            }

            //if(mapa[y][x].estadoBandera >= 0){
            //
            //}

            x++;
        }
        y++;
    }
}

// Funcion que coloca estados en las casillas
void casillaEstado(Juego *juego , Coord *minasCoord , int minas , int gX , int gY){

    if (gX < 0 || gX >= juego->dimMapa || gY < 0 || gY >= juego->dimMapa)
        return;

    Casilla *casillaSeleccionada = &juego->mapa[gY][gX];
    Casilla *casillaBandera = &juego->mapa[gY][gX];

    // No hacer nada si ya está presionada o tiene bandera
    if (casillaSeleccionada->presionada || casillaBandera->estadoBandera != 0)
        return;

    casillaSeleccionada->presionada = true;
    juego->cantCasillasPresionadas++;

    // Juego Perdido
    /* Mover
    if (casillaSeleccionada->estado == -1)
    {
        juego->finPartida = true;
        dibujar(renderer, PIXELES_X_LADO, mine2, gX, gY, picords->x, picords->y);

        // Mostrar todas las bombas
        for (int i = 0; i < minas; i++)
        {

            int mX = minasCoord[i].x;
            int mY = minasCoord[i].y;

            if (gX != mX && gY != mY)
                dibujar(renderer, PIXELES_X_LADO, mine, mX, mY, picords->x, picords->y);
        }
        return;
    }
    */

    // Dibuja numero y termina

    if (casillaSeleccionada->estado > 0 || casillaSeleccionada->estado == -1){

        //dibujar(renderer, PIXELES_X_LADO, eleccion(casillaSeleccionada->estado), gX, gY, picords->x, picords->y);
        return;
    }
    //dibujar(renderer, PIXELES_X_LADO, square2, gX, gY, picords->x, picords->y);

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0) continue; // Evita repetirse a sí mismo
            casillaEstado(juego , minasCoord , minas , gX + i , gY + j);
        }
    }
}

// Funcion para colocar bandera
void casillaBandera(Juego *juego, int xG, int yG){

    if (xG < 0 || xG >= juego->dimMapa || yG < 0 || yG >= juego->dimMapa)
        return;

    Casilla **mapa = juego->mapa;
    //Realizo una iteracion ciclica con el resto
    // 1%3 = 1, 2%3 = 2, 3%3 = 0;
    mapa[yG][xG].estadoBandera = (mapa[yG][xG].estadoBandera + 1) % 3;

    //Suma y resta de bombas dependiendo el caso
    if (mapa[yG][xG].estadoBandera == 1)
        (juego->cantMinasEnInterfaz)--;
    else if (mapa[yG][xG].estadoBandera == 2)
        (juego->cantMinasEnInterfaz)++;

    //dibujar(renderer, PIXELES_X_LADO, eleccionBandera(mapa[gY][gX].estadoBandera), gX, gY, picord->x, picord->y);
    printf("Estado (%i , %i): %i\n",xG,yG,mapa[yG][xG].estadoBandera);
}

//funciones Log
void setLog(Log* log, int coordX, int coordY, char tipoEvento[80])
{
    time_t ahora = time(NULL);
    struct tm* aux = localtime(&ahora);
    log->fechaHora = *aux; // Asignar la fecha y hora actual al log
    strcpy(log->tipoEvento, tipoEvento);
    log->coordXY[0] = coordX;
    log->coordXY[1] = coordY;
}

// Función para abrir un archivo y manejar errores
FILE* abrirArchivo(const char* nombre, const char* modo) {
    FILE* archivo = fopen(nombre, modo);
    if (!archivo) {
        //setLog(&log, -1, -1, "Error al abrir el archivo.");
        //escribirArchivoLog(archivoLog, &log);
        puts("Error al abrir el archivo.");
        return NULL;
    }
    return archivo;
}