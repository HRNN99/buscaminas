# Instalacion de librerias
* Descargar la libreria SDL2_ttf y SDL2_mixer desde:  
[SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.tar.gz)  
[SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-mingw.zip)
* Agregarlo a la carpeta del proyecto
* Copiar el archivo SDL2_ttf.h y SDL2_mixer.h dentro de la carpeta de SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
* Copiar el archivo SDL2_ttf.dll SDL2_mixer.dll dentro de la carpeta donde esta el .exe buscaminas\bin\Debug
* Dentro de codeblocks agregar en las build options -> Linker settings -> other linker options "-lSDL2_ttf -lSDL2_mixer" quedando asi: 
> "-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer"
* Colocar el directorio SDL2_ttf-2.22.0\x86_64-w64-mingw32\lib y SDL2_mixer-2.8.1\x86_64-w64-mingw32\lib en  build options -> search directories -> linker. O en su defecto copiar lo de esta carpeta dentro de la carpeta lib de SDL2
