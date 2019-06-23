//
// Created by sneeuwpop on 24-6-19.
//

#pragma once

#include "camera.h"

class FlyingCamera : public Camera
{
public:
    void update(float deltaTime);

private:
    float speedMultiplier = 1.0;
    float rotationSpeed = 90;
};