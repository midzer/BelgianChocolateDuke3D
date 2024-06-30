# Emscripten

## Build

```
mkdir build
cd build
emcmake cmake ..
emmake make
```

##  Link

```
emcc -flto -O3 -fno-rtti -fno-exceptions Engine/CMakeFiles/Engine.dir/src/*.o Game/CMakeFiles/Game.dir/src/audiolib/*.o Game/CMakeFiles/Game.dir/src/midi/*.o Game/CMakeFiles/Game.dir/src/*.o -o index.html -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=mid -sASYNCIFY -sENVIRONMENT=web --preload-file DUKE3D.GRP --preload-file duke3d.cfg --preload-file dgguspat/ --preload-file timidity.cfg --closure 1 -sEXPORTED_RUNTIME_METHODS=['allocate']
```
