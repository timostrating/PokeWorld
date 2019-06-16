cmake_minimum_required(VERSION 3.0)

project(poke)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

file(GLOB_RECURSE source src/*)
add_executable(make_emscripten ${source})

set_target_properties(make_emscripten PROPERTIES LINKER_LANGUAGE CXX)

set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE) # https://stackoverflow.com/questions/12264299/cmake-on-linux-target-platform-does-not-support-dynamic-linking

set_property(TARGET make_emscripten PROPERTY CXX_STANDARD 17)
set_property(TARGET make_emscripten PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# ---GLFW---

# disable GLFW docs, tests and examples:
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(./external/glfw-3.2.1)
#include_directories(./external/glfw-3.2.1)


# ---GLM---
add_subdirectory(./external/glm-0.9.9.5/glm)
include_directories(./external/glm-0.9.9.5/glm)


# ---GLAD---
add_library(glad ./external/glad/include/glad/glad.h ./external/glad/src/glad.c)
target_include_directories(glad PUBLIC ./external/glad/include/)


# -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
#set(CMAKE_CXX_FLAGS  "-no-pie -O3")

#target_link_libraries(gameutils glad glfw glm)
#target_include_directories(gameutils PUBLIC source/)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR} POST_BUILD COMMAND bash -c "echo 'JHEEE\nJHEEE\nJHEEE\nJHEEE\nJHEEE\nJHEEE'" VERBATIM)

set_target_properties(make_emscripten PROPERTIES SUFFIX ".html")
set_target_properties(make_emscripten PROPERTIES LINK_FLAGS "-Werror -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm'")

target_link_libraries(make_emscripten glad glm)
