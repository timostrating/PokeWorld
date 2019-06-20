cmake_minimum_required(VERSION 3.0)

project(poke)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

file(GLOB_RECURSE source ../src/*)

#set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE) # https://stackoverflow.com/questions/12264299/cmake-on-linux-target-platform-does-not-support-dynamic-linking

find_package(OpenGL REQUIRED)

# ---GLFW---
# disable GLFW docs, tests and examples:
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#
#add_subdirectory(../external/glfw-3.2.1 ./bindir)
#include_directories(../external/glfw-3.2.1)


# ---GLM---
add_subdirectory(../external/glm-0.9.9.5/glm ./bindir2)
include_directories(../external/glm-0.9.9.5/glm)


# ---ImGUI---
set(IMGUI_DIR ../external/imgui-1.71)
include_directories(${IMGUI_DIR})

#add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD) # IMGUI recuires that we say that we use glad on the desktop
# ImGui detects EMSCRIPTEN automaticly and use #include <GLES2/gl2.h> instead


# ---GLAD---
add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)


# ---END---
add_executable(make_emscripten
            ${source}
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
            ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_demo.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp)

set_target_properties(make_emscripten PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET make_emscripten PROPERTY CXX_STANDARD 17)
set_property(TARGET make_emscripten PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(make_emscripten glad glm)


set_target_properties(make_emscripten PROPERTIES SUFFIX ".html")
set_target_properties(make_emscripten PROPERTIES LINK_FLAGS "-Werror -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' -s ASSERTIONS=2")

target_link_libraries(make_emscripten glad glm)
