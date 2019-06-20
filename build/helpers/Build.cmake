cmake_minimum_required(VERSION 3.0)

project(poke)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

# Detect if we use emscripten. Use it like so if (${__EMSCRIPTEN__})   OR   if (NOT ${__EMSCRIPTEN__})
set(__EMSCRIPTEN__ (CMAKE_CURRENT_SOURCE_DIR MATCHES "/build_emscripten")) # MATCHES does a regex on the uri


# --------------------------------------------------------SOURCE--------------------------------------------------------

file(GLOB_RECURSE source ../../src/*)

# https://stackoverflow.com/questions/12264299/cmake-on-linux-target-platform-does-not-support-dynamic-linking
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

set(OpenGL_GL_PREFERENCE "GLVND") # We dont want the legacy openGl
find_package(OpenGL REQUIRED)


# ---------------------------------------------------------GLFW---------------------------------------------------------

if (NOT ${__EMSCRIPTEN__})
    # disable GLFW docs, tests and examples:
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(../../external/glfw-3.2.1 ./bindir)
    include_directories(../../external/glfw-3.2.1)
endif ()


# ---------------------------------------------------------GLM----------------------------------------------------------

add_subdirectory(../../external/glm-0.9.9.5/glm ./bindir2)
include_directories(../../external/glm-0.9.9.5/glm)


# ---------------------------------------------------------ImGUI--------------------------------------------------------

set(IMGUI_DIR ../../external/imgui-1.71)
include_directories(${IMGUI_DIR})

if (NOT ${__EMSCRIPTEN__})
    add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD) # IMGUI recuires that we say that we use glad on the desktop
    # ImGui detects EMSCRIPTEN automaticly and use #include <GLES2/gl2.h> instead
endif ()


# --------------------------------------------------------GLAD----------------------------------------------------------

add_library(glad ../../external/glad/include/glad/glad.h ../../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../../external/glad/include/)


# ---------------------------------------------------------END----------------------------------------------------------

add_executable(TinyPokemonWorld
        ${source}
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp)

set_target_properties(TinyPokemonWorld PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET TinyPokemonWorld PROPERTY CXX_STANDARD 17)
set_property(TARGET TinyPokemonWorld PROPERTY CXX_STANDARD_REQUIRED ON)


if (${__EMSCRIPTEN__})

    set_target_properties(TinyPokemonWorld PROPERTIES SUFFIX ".html")
    set_target_properties(TinyPokemonWorld PROPERTIES LINK_FLAGS "-Werror -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' -s ASSERTIONS=2")
    target_link_libraries(TinyPokemonWorld glad glm)

else()

    target_link_libraries(TinyPokemonWorld glad glm glfw)

    # -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
    set(CMAKE_CXX_FLAGS  "-no-pie -O3")

endif()