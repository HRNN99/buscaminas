#ifndef SONIDO_H
#define SONIDO_H


#include <SDL2/SDL_mixer.h>


#define MIXER_FLAGS MIX_INIT_OGG

struct Juego;

//declaracion de funciones
int iniciarSonido();
int cargarSonido(const char *ruta, Mix_Chunk **sonido, int volumen);
int cargarMusica(const char *ruta, Mix_Music **musica, int volumen);
void iniciarMusicaMenu(Mix_Music **musica);


void iniciarMusicaJuego(Mix_Music **musica);

void detenerMusica(void);
#endif // SONIDO_H