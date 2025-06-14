#ifndef SONIDO_H
#define SONIDO_H


#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "juego.h"

#define MIXER_FLAGS MIX_INIT_OGG


//declaracion de funciones
int iniciarSonido();
int cargarSonido(const char *ruta, Mix_Chunk **sonido);

#endif // SONIDO_H_INCLUDED
