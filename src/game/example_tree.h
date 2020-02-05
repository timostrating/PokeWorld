//
// Created by sneeuwpop on 05-02-20.
//

#pragma once

#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <imgui.h>
#include <stack>
#include "../util/interfaces/game_object.h"
#include "lsystem.h"

using namespace glm;

class ExampleTree
{
public:
    Gizmos gizmos;

    const static int cacheSize = 100;
    vec3 plusCache[cacheSize];
    vec3 minCache[cacheSize];

    const float angle = radians(18.f);
    mat4 transform = translate(mat4(1), vec3(-4, 1, -1));
    double time = 0;

    std::string buf = std::string("F[-x[+x]++x]") + std::string(1000, '\0');
    int applyNtimes = 5;
    bool twoD = false;
    float growthSize = 3.0f;

    ExampleTree()
    {
        for (int i=0; i<cacheSize; i++) {
            plusCache[i] = MATH::randomPointOnSphere(35, 180);
            minCache[i] = MATH::randomPointOnSphere(35, -180);
        }
    }

    void render() {
        LSystem lSystem = LSystem("X");
        lSystem.addPattern('X', buf); // "F[-X][+X],FX"

        vec3 curPoint = VEC3::ZERO, oldPoint = VEC3::ZERO;
        vec3 direction = VEC3::Y;

        std::stack<vec3> memory = std::stack<vec3>();

        lSystem.applyNtimes(applyNtimes);
        std::string str = lSystem.getStr();
        for (int i = 0; i < str.length(); i++) {
            switch (toupper(str[i])) {
                case 'F': oldPoint = vec3(curPoint); curPoint += (direction) * growthSize; gizmos.drawLine(oldPoint, curPoint, COLOR::WHITE, transform); break;
                case '+': direction = glm::rotate(direction, angle, VEC3::Z); break;
                case '-': direction = glm::rotate(direction, -angle, VEC3::Z); break;
                case '/': direction = glm::rotate(direction, angle, VEC3::X); break;
                case '\\': direction = glm::rotate(direction, -angle, VEC3::X); break;
                case '!': direction = twoD ? vec3(plusCache[i % cacheSize].x, plusCache[i % cacheSize].y, 0) : plusCache[i % cacheSize]; break;
                case '&': direction = twoD ? vec3(minCache[i % cacheSize].x, minCache[i % cacheSize].y, 0) : minCache[i % cacheSize]; break;
                case '[': memory.push(curPoint); memory.push(direction); break;
                case ']':
                    if (memory.empty()) break;
                    direction = memory.top(); memory.pop();
                    curPoint = memory.top(); memory.pop(); break;
            }
        }
    }

    void renderGui() {
        ImGui::Begin("LSystem pattern");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Change the text and see what happens");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text(" F : Move forward");
        ImGui::Text(" X : Replace this X with the pattern");
        ImGui::Text(" + : Rotate right \n - : Rotate left");
        if(!twoD) ImGui::Text(" / : Yaw right \n \\ : Yaw left");
        ImGui::Text(" ! : Random rightish \n & : Random leftisch");
        ImGui::Text(" [ : Store current location \n ] : Pop last stored location");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        //        char *buf = new char[10];
        ImGui::InputText("pattern", &buf[0], buf.size()+ 1 );
        if (ImGui::Button("Reset", ImVec2(200, 20))) buf = std::string("F[-x[+x]++x]") + std::string(1000, '\0');
        ImGui::Text("Times to apply pattern");
        ImGui::SliderInt("", &applyNtimes, 1, 6);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SliderFloat("Growth size", &growthSize, 1, 5);
        ImGui::Checkbox("2d instead of 3d", &twoD);
        ImGui::End();
    }
};


