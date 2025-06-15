#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "sonido.h"
#include "juego.h"

int iniciarSonido(Sonido *sonido) {

    int mix_init = Mix_Init(MIX_INIT_MP3);

    if (mix_init != MIX_INIT_MP3) {
        fprintf(stderr, "Error al inicializar Mixer: %s\n", Mix_GetError());
        return -1; // Error al inicializar Mix_Init
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)) {
        fprintf(stderr, "Error al abrir el audio: %s\n", Mix_GetError());
        return -1; // Error al abrir el audio
    }
    sonido->sonidoMina = NULL;
    sonido->sonidoClick = NULL;
    sonido->sonidoBandera = NULL;
    sonido->sonidoCat = NULL;
    sonido->sonidoPerder = NULL;
    sonido->sonidoFlecha = NULL;
    sonido->sonidoEnter = NULL;
    sonido->musicaFondo = NULL;
    sonido->musicaMenu = NULL;
    return 0;

}

int cargarSonido(const char *ruta, Mix_Chunk **sonido, int volumen) {

    *sonido = Mix_LoadWAV(ruta);
    if (!sonido) {
        fprintf(stderr, "Error al cargar el sonido: %s\n", Mix_GetError());
        return -1; // Error al cargar el sonido
    }
    Mix_VolumeChunk(*sonido,volumen); // Ajusta el volumen del sonido
    

    return 0; // �xito
}

int cargarMusica(const char *ruta, Mix_Music **musica, int volumen) {

    *musica = Mix_LoadMUS(ruta);
    if (!musica) {
        fprintf(stderr, "Error al cargar la musica: %s\n", Mix_GetError());
        return -1; // Error al cargar la musica
    }
    Mix_VolumeMusic(volumen);
    return 0; // �xito
}


void iniciarMusicaMenu(Mix_Music **musica) {
    detenerMusica();
    Mix_PlayMusic(*musica, -1); // -1 for infinite loop
}

void iniciarMusicaJuego(Mix_Music **musica) {
    detenerMusica();
    Mix_PlayMusic(*musica, -1);
}

void detenerMusica(void) {
    if(Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

