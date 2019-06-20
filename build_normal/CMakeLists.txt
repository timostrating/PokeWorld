cmake_minimum_required(VERSION 3.0)

project(poke)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

file(GLOB_RECURSE source ../src/*)

set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE) # https://stackoverflow.com/questions/12264299/cmake-on-linux-target-platform-does-not-support-dynamic-linking

find_package(OpenGL REQUIRED)

# ---GLFW---
# disable GLFW docs, tests and examples:
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../external/glfw-3.2.1 ./bindir)
include_directories(../external/glfw-3.2.1)


# ---GLM---
add_subdirectory(../external/glm-0.9.9.5/glm ./bindir2)
include_directories(../external/glm-0.9.9.5/glm)


# ---ImGUI---
set(IMGUI_DIR ../external/imgui-1.71)
include_directories(${IMGUI_DIR})


# ---GLAD---
add_library(glad ../external/glad/include/glad/glad.h ../external/glad/src/glad.c)
target_include_directories(glad PUBLIC ../external/glad/include/)


# ---END---
# ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp  // We hard copied an edited version of this file into the project
add_executable(make_normal ${source} ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp      ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_widgets.cpp)

set_target_properties(make_normal PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET make_normal PROPERTY CXX_STANDARD 17)
set_property(TARGET make_normal PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(make_normal glad glm glfw)

# -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
set(CMAKE_CXX_FLAGS  "-no-pie -O3")

