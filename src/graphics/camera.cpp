//
// Created by sneeuwpop on 23-6-19.
//

#include "camera.h"

#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <glad/glad.h>
#include <iostream>

using namespace glm;


Camera *Camera::main = nullptr;

Camera::Camera()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!main)
        main = this;
}

void Camera::update()
{
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

//    projection = ortho(
//            -1.0, 1.0, -1.0, 1.0,
//            nearClippingPlane,              // near clipping plane
//            farClippingPlane                // far clipping plane
//    );

    combined = projection * view; // Remember, matrix multiplication is the other way around
}

void Camera::lookAt(vec3 target)
{
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

void Camera::rotate(vec3 axis, float degrees)
{
    float rads = radians(degrees);
    direction = glm::rotate(direction, rads, axis);
    right = glm::rotate(right, rads, axis);
    up = glm::rotate(up, rads, axis);
}

void Camera::translate(vec3 target)
{
    position += target;
}

void Camera::debugDraw()
{
    gizmos.drawLine(0.001f * VEC3::Y, 20.0f * VEC3::X, vec4(VEC3::X, 1.0));
    gizmos.drawLine(0.001f * VEC3::Y, 20.0f * VEC3::Y, vec4(VEC3::Y, 1.0));
    gizmos.drawLine(0.001f * VEC3::Y, 20.0f * VEC3::Z, vec4(VEC3::Z, 1.0));

    for (int x=-10; x<=10; x++) { gizmos.drawLine(vec3(x, 0 ,-10), vec3(x, 0 ,10), vec4(1.0, 1.0, 1.0, 0.2f)); }
    for (int z=-10; z<=10; z++) { gizmos.drawLine(vec3(-10, 0 ,z), vec3(10, 0 ,z), vec4(1.0, 1.0, 1.0, 0.2f)); }
}

void Camera::resize(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
}

void Camera::invertPitch()
{
    direction.y *= -1;
    lookAt(direction);
}

void Camera::saveState()
{
    oldDirection = direction;
    oldPos = position;
}

void Camera::restoreState()
{
    position = oldPos;
    lookAt(position + oldDirection);
    update();
}
