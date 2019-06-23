//
// Created by sneeuwpop on 23-6-19.
//

#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

using namespace glm;


Camera::Camera() {
    glEnable(GL_DEPTH_TEST); // TODO permanent place
}

void Camera::update() {
    view = lookAt(
            position,       // camera position
            vec3(0, 0, 0),  // camera loops at
            up              // camera Up vector
    );
    projection = perspective(
            radians(45.0),  // Field of View
            16.0 / 9.0,     // Aspect Ratio
            0.1,            // near clipping plane
            100.0           // far clipping plane
    );

    combined = projection * view; // Remember, matrix multiplication is the other way around
}

Camera::~Camera() {

}

