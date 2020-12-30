cmake_minimum_required(VERSION 3.0)

project(poke CXX)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

# Detect if we use emscripten. Use it like so if (${__EMSCRIPTEN__})   OR   if (NOT ${__EMSCRIPTEN__})
set(__EMSCRIPTEN__ (CMAKE_CURRENT_SOURCE_DIR MATCHES "/build/emscripten")) # MATCHES does a regex on the uri
set(EXTERNAL_DIR ../../external)
set(ASSETS_DIR ../../assets)
set(PROJECT_NAME TinyPokemonWorld)


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

    add_subdirectory(${EXTERNAL_DIR}/glfw ./bin/glfw)
    include_directories(${EXTERNAL_DIR}/glfw)
endif ()


# ---------------------------------------------------------GLM----------------------------------------------------------

add_subdirectory(${EXTERNAL_DIR}/glm/glm ./bin/glm)
include_directories(${EXTERNAL_DIR}/glm/glm)


# ---------------------------------------------------------ImGUI--------------------------------------------------------


set(IMGUI_DIR ${EXTERNAL_DIR}/imgui)
include_directories(${IMGUI_DIR})

set(IMGUI_NODE_EDITOR_DIR ${EXTERNAL_DIR}/imgui-node-editor)
include_directories(${IMGUI_NODE_EDITOR_DIR})

if (NOT ${__EMSCRIPTEN__})
    add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD) # IMGUI recuires that we say that we use glad on the desktop
    # ImGui detects EMSCRIPTEN automaticly and use #include <GLES2/gl2.h> instead
endif ()


# --------------------------------------------------------GLAD----------------------------------------------------------

add_library(glad ${EXTERNAL_DIR}/glad/include/glad/glad.h ${EXTERNAL_DIR}/glad/src/glad.c)
target_include_directories(glad PUBLIC ${EXTERNAL_DIR}/glad/include/)


# ---------------------------------------------------------END----------------------------------------------------------

add_executable(${PROJECT_NAME}
        ${source}
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_NODE_EDITOR_DIR}/crude_json.cpp
        ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor_api.cpp
        ${IMGUI_NODE_EDITOR_DIR}/imgui_canvas.cpp
        ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor.cpp
        ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor_api.cpp
    )

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)


if (${__EMSCRIPTEN__})

    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Werror -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' --preload-file ${ASSETS_DIR} --shell-file ../helpers/index.html")
    target_link_libraries(${PROJECT_NAME} glad glm)

else()

    target_link_libraries(${PROJECT_NAME} glad glm glfw)

    # -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
    set(CMAKE_CXX_FLAGS  "-no-pie -O3")

endif()