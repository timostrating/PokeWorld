//
// Created by sneeuwpop on 23-6-19.
//

#include "camera.h"

#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace glm;


Camera *Camera::main = 0;

Camera::Camera() {
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    if (!main)
        main = this;
}

void Camera::update() {
//    std::cout << "position " << VEC3::toString(position) << " \n";
//    std::cout << "direction " << VEC3::toString(direction) << " \n";
//    std::cout << "right " << VEC3::toString(right) << " \n";
//    std::cout << "up " << VEC3::toString(up) << " \n\n\n";

    view = glm::lookAt(
            position,                 // camera position
            position + direction,     // camera looks at
            up                        // camera Up vector
    );
    projection = perspective(
            radians(fov),                   // Field of View
            double(width) / double(height), // Aspect Ratio
            nearClippingPlane,              // near clipping plane
            farClippingPlane                // far clipping plane
    );

    combined = projection * view; // Remember, matrix multiplication is the other way around
}

void Camera::lookAt(vec3 target) {
    /// | | | | | | |
    /// | | | | | | |    pos       (0,  0,  2)
    /// | | | | | | |    lookAt    (0,  0,  0)
    /// | | | X | | |    direction (0,  0, -1)
    /// | | | | | | |
    /// | | | | | | |
    /// | | | C | | |

    direction = normalize(target - position);
    right = normalize(cross(direction, VEC3::Y));
    up = normalize(cross(right, direction));
}

void Camera::rotate(vec3 axis, float degrees) {
    float rads = radians(degrees);
    direction = glm::rotate(direction, rads, axis);
    right = glm::rotate(right, rads, axis);
    up = glm::rotate(up, rads, axis);
}

void Camera::translate(vec3 target) {
    position += target;
}