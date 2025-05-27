# Instalacion de librerias
* Descargar la libreria SDL2_ttf desde
[SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.tar.gz)

* Agregarlo a la carpeta del proyecto
* Copiar el archivo SDL2_ttf.h dentro de la carpeta de SDL2-2.0.22\x86_64-w64-mingw32\include\SDL2
* Copiar el archivo SDL2_ttf.dll dentro de la carpeta donde esta el .exe buscaminas\bin\Debug
* Dentro de codeblocks agregar en las build options -> Linker settings -> other linker options "-lSDL2_ttf" quedando asi: 
> "-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf"
