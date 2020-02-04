//
// Created by sneeuwpop on 31-01-20.
//

#pragma once

#include <imgui.h>
#include "../util/interfaces/game_object.h"
#include "../graphics/shader_program.h"
#include "../util/splines/line.h"

using namespace glm;

class Route : public GameObject
{
    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/terrain-route.vert", "shaders/terrain-route.frag");

    mat4 transform = translate(mat4(1), vec3(0, 1, 0));

    float a=-18,b=-5,c=-8,d=-3,e=-2,f=7,g=0,h=19;
    Line line = Line({scale(rotate(translate(mat4(1), vec3(a, 0, b)), radians(280.0f), VEC3::Y), vec3(0.8, 1, 0.8)),
                      scale(rotate(translate(mat4(1), vec3(c, 0, d)), radians(230.0f), VEC3::Y), vec3(1.1, 1, 1.1)),
                      scale(rotate(translate(mat4(1), vec3(e, 0, f)), radians(210.0f), VEC3::Y), vec3(1.2, 1, 1.2)),
                      scale(rotate(translate(mat4(1), vec3(g, 0, h)), radians(180.0f), VEC3::Y), vec3(0.8, 1, 0.8))});

    std::vector<vec3> points = {vec3(-1, 0.01, 0), vec3(1, 0.01, 0)};

    SharedMesh mesh = SharedMesh(line.wrapMeshAround(&points, true, true, false));
    GLuint MVP, u_time;

public:
    Route()
    {
        shader.use();
        MVP = shader.uniformLocation("MVP");
        u_time = shader.uniformLocation("u_time");

        VertexBuffer::uploadSingleMesh(mesh);

//        colorPickerData = new ColorPickerData{mesh, transform};
    }

    void render(float time);
    void debugRender(Gizmos* gizmos)
    {
        vec3 p1=vec3(a, 1.01, b), p2=vec3(c, 1.01, d), p3=vec3(e, 1.01, f), p4=vec3(g, 1.01, h);
        gizmos->drawLine(p1, p2);
        gizmos->drawLine(p2, p3);
        gizmos->drawLine(p3, p4);
    }

    void renderGui()
    {
        ImGui::SliderFloat("a", &a, -25.0, 25.0);
        ImGui::SliderFloat("b", &b, -25.0, 25.0);
        ImGui::SliderFloat("c", &c, -25.0, 25.0);
        ImGui::SliderFloat("d", &d, -25.0, 25.0);
        ImGui::SliderFloat("e", &e, -25.0, 25.0);
        ImGui::SliderFloat("f", &f, -25.0, 25.0);
        ImGui::SliderFloat("g", &g, -25.0, 25.0);
        ImGui::SliderFloat("h", &h, -25.0, 25.0);
    }

};

