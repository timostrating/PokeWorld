# TinyPokemonWorld

WARNING: This is a work in progress Portfolio item.

<p align="center">
  <img src="https://i.imgur.com/GvXiFLX.png" alt="image" width=>
</p>


[Emscpriten](https://emscripten.org/) + [GLFW3](https://www.glfw.org/) + [GLAD](https://www.glfw.org/) + [GLM](https://github.com/g-truc/glm/releases) + [ImGui](https://github.com/ocornut/imgui) + [stb](https://github.com/nothings/stb) + [personal unit test code](https://github.com/timostrating/unity_test)

## Setup
OPTIONAL: Setup [Emscpriten](https://emscripten.org/)
1. To manage the dependencies I use submodules. So get them by recursively cloning this repo or run  `$ git submodule update --init --recursive` after you cloned it
2. Download [GLAD with opengl es 3.2](http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3Dnone&api=gles1%3Dnone&api=gles2%3D3.2&api=glsc2%3Dnone&language=c&loader=on) and place the unzipped files in `./external/glad` 

## Building it

Build the (optional) Emscpriten version

`$ cd build/emscripten` <br/>
`$ emconfigure cmake ./` <br/>
`$ make` <br/>
`$ emrun out/TinyPokemonWorld.html` <br/>


Or build the normal desktop version

`$ cd build/desktop` <br/>
`$ cmake ./` <br/>
`$ make` <br/>

## Inspired by

https://github.com/hilkojj/cpp-GAME-utils <br/>
https://github.com/hilkojj/cpp-GAME-utils-template

https://github.com/emscripten-core/emscripten/tree/incoming/tests

https://www.amazon.com/Computer-Graphics-OpenGL-Donald-Hearn/dp/0130153907
