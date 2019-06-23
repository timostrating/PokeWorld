//
// Created by sneeuwpop on 24-6-19.
//

#include "flying_camera.h"
#include "../util/input/keyboard.h"
using namespace INPUT;

///
/// CREDIT TO: https://github.com/hilkojj/cpp-game-utils/blob/master/source/utils/camera/flying_camera_controller.cpp
///
void FlyingCamera::update(float deltaTime) {
//    speedMultiplier += MouseInput::yScroll * .5; // TODO: add a
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


    if (KEYBOARD::pressed(GLFW_KEY_LEFT_SHIFT))
        position.y -= deltaTime * speedMultiplier;

    if (KEYBOARD::pressed(GLFW_KEY_SPACE))
        position.y += deltaTime * speedMultiplier;

//    if (MouseInput::deltaMouseX != 0)
//        rotate(MouseInput::deltaMouseX / gu::width * -100 * mouseSensivity, mu::Y);
//
//    if (MouseInput::deltaMouseY != 0)
//        rotate(MouseInput::deltaMouseY / gu::height * -100 * mouseSensivity, right);

    Camera::update();
}
