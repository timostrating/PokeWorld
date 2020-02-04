//
// Created by sneeuwpop on 24-6-19.
//

#include <iostream>
#include "flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/input/mouse.h"
using namespace INPUT;
using namespace MATH;

// CREDIT TO: https://github.com/hilkojj/cpp-game-utils/blob/master/source/utils/camera/flying_camera_controller.cpp
void FlyingCamera::debugUpdate(float deltaTime)
{
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


void FlyingCamera::update(float deltaTime)
{
    velocityX -= 0.5f * deltaTime;
    if (velocityX < 0.0f) velocityX = 0.0f;

    velocityY -= 0.5f * deltaTime;
    if (velocityY < 0.0f) velocityY = 0.0f;


    newX = INPUT::MOUSE::getMousePosX();
    if (INPUT::MOUSE::leftClick())
        velocityX = (oldX - newX) / static_cast<float>(width);
    s += velocityX * PI;
    oldX = newX;

    newY = INPUT::MOUSE::getMousePosY();
    if (INPUT::MOUSE::leftClick())
        velocityY = (oldY - newY) / static_cast<float>(width);
    t += velocityY * PI;
    t = clamp(t, 0.01f, 0.99f * PI);
    oldY = newY;

#ifdef __EMSCRIPTEN__
    r += INPUT::MOUSE::getScrollDelta() * 5.0;
#else
    r -= INPUT::MOUSE::getScrollDelta() * 5.0;
#endif
    r = clamp(r, 20.0f, 150.0f);


    position = vec3(
            sin(s) * sin(t) * r,  // y
            cos(t) * r,           // z
            cos(s) * sin(t) * r   // x
        );
    lookAt(vec3(0, 1, 0));

    Camera::update();
}