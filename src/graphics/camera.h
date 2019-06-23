//
// Created by sneeuwpop on 23-6-19.
//

#pragma once

#include <glm/glm.hpp>
#include "../util/interfaces/resizeable.h"
#include "../util/math/math.h"
using namespace glm;


class Camera : Resizeable
{
public:
    Camera();
    Camera(double FOV, int screenWidth, int screenHeight) : Camera() {
        fov = FOV;
        width = screenWidth;
        height = screenHeight;
    }
    Camera(double FOV, int screenWidth, int screenHeight, double nearClipping, double farClipping) : Camera(FOV, screenHeight, screenHeight) {
        nearClippingPlane = nearClipping;
        farClippingPlane = farClipping;
    }

    int width = 16, height = 9;
    double fov = 45;
    double nearClippingPlane = 0.1;
    double farClippingPlane = 100.0;

    vec3 position = VEC3::ZERO;
    vec3 direction = -VEC3::Z;
    vec3 right = VEC3::X;
    vec3 up = VEC3::Y;

    mat4 projection = MAT4::IDENTITY;
    mat4 view = MAT4::IDENTITY;
    mat4 combined = MAT4::IDENTITY; // projection * view

    void update();

    void lookAt(float x, float y, float z) { lookAt(vec3(x,y,z)); }
    void lookAt(vec3 target);

//    void rotate(Matrix4 transform);
//    void rotate(Quaternion quat);
    void rotate(vec3 axis, float degrees);
//    void rotateAround(vec3 point, vec3 axis, float angle);

    void translate(float x, float y, float z) { translate(vec3(x,y,z)); }
    void translate(vec3 target);


    void resize(int newWidth, int newHeight) {
        width = newWidth;
        height = newHeight;
    }
};