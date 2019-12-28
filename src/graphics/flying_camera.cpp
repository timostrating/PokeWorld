//
// Created by sneeuwpop on 24-6-19.
//

#include <iostream>
#include "flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/input/mouse.h"
using namespace INPUT;


// CREDIT TO: https://github.com/hilkojj/cpp-game-utils/blob/master/source/utils/camera/flying_camera_controller.cpp
void FlyingCamera::update(float deltaTime) {
    speedMultiplier += MOUSE::getScrollDelta();
    if (speedMultiplier < 0) speedMultiplier = 0;

    if (KEYBOARD::pressed(GLFW_KEY_W))
        position += direction * glm::vec3(deltaTime * speedMultiplier);

    if (KEYBOARD::pressed(GLFW_KEY_S))
        position += direction * glm::vec3(-deltaTime * speedMultiplier);

    if (KEYBOARD::pressed(GLFW_KEY_D))
        position += right * glm::vec3(deltaTime * speedMultiplier);

    if (KEYBOARD::pressed(GLFW_KEY_A))
        position += right * glm::vec3(-deltaTime * speedMultiplier);

    if (KEYBOARD::pressed(GLFW_KEY_Q))
        rotate(VEC3::Y, rotationSpeed * deltaTime);

    if (KEYBOARD::pressed(GLFW_KEY_E))
        rotate(VEC3::Y, -rotationSpeed * deltaTime);


    if (KEYBOARD::pressed(GLFW_KEY_F))
        position.y -= deltaTime * speedMultiplier;

    if (KEYBOARD::pressed(GLFW_KEY_R))
        position.y += deltaTime * speedMultiplier;

//    if (MOUSE::deltaMouseX != 0)
//        rotate(MOUSE::deltaMouseX / gu::width * -100 * mouseSensivity, VEC3::Y);
//
//    if (MOUSE::deltaMouseY != 0)
//        rotate(MOUSE::deltaMouseY / gu::height * -100 * mouseSensivity, right);

    Camera::update();
}
