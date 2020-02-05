//
// Created by sneeuwpop on 24-6-19.
//

#pragma once

#include "camera.h"
#include "../util/input/mouse.h"

class FlyingCamera : public Camera
{
public:
    void debugUpdate(float deltaTime);
    void update(float deltaTime);

private:
    float speedMultiplier = 20.0;
    float rotationSpeed = 90;

    float s = 0;
    float t = 1;

    float velocityX = 0;
    int oldX = INPUT::MOUSE::getMousePosX();
    int newX = oldX;

    float velocityY = 0;
    int oldY = INPUT::MOUSE::getMousePosY();
    int newY = oldY;

    float r = 100.0;
};