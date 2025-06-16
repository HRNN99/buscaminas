#ifndef SONIDO_H
#define SONIDO_H


#include <SDL2/SDL_mixer.h>


#define MIXER_FLAGS MIX_INIT_OGG

typedef struct {
    Mix_Chunk *sonidoMina;
    Mix_Chunk *sonidoClick;
    Mix_Chunk *sonidoBandera;
    Mix_Chunk *sonidoCat;
    Mix_Chunk *sonidoPerder;
    Mix_Chunk *sonidoFlecha;
    Mix_Chunk *sonidoEnter;
    Mix_Music *musicaFondo;
    Mix_Music *musicaMenu;
} Sonido;

//declaracion de funciones
int iniciarSonido(Sonido *sonido);
int cargarSonido(const char *ruta, Mix_Chunk **sonido, int volumen);
int cargarMusica(const char *ruta, Mix_Music **musica, int volumen);
void iniciarMusica(Mix_Music **musica);

void detenerMusica(void);
#endif // SONIDO_H