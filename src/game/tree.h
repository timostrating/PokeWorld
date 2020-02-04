//
// Created by sneeuwpop on 28-01-20.
//

#pragma once

#include <stack>
#include "../util/interfaces/game_object.h"
#include "../util/splines/line.h"
#include "lsystem.h"
#include "sky.h"

class Tree : public GameObject
{
public:
    SharedMesh tree = SharedMesh(new Mesh(0,0, VA_POSITION_NORMAL));
    mat4 transform;
    ShaderProgram flatShader = ShaderProgram::fromAssetFiles("shaders/tree.vert", "shaders/tree.frag");
    GLuint MVP, u_time;
    const float angle = radians(15.f);

    std::vector<Line> lines;

    Tree(mat4 transform, std::string pattern = "F[-x[+&x]!]") : transform(transform)
    {
        MVP = flatShader.uniformLocation("MVP");
        u_time = flatShader.uniformLocation("u_time");

        LSystem lSystem = LSystem("X");
        lSystem.addPattern('X', pattern);

        vec3 curPoint = VEC3::ZERO, oldPoint = VEC3::ZERO;
        vec3 curDirection = VEC3::Y, oldDirection = VEC3::Y;


        std::stack<vec3> memory = std::stack<vec3>();

        lSystem.applyNtimes(3);
        std::string str = lSystem.getStr();
        for (int i = 0; i < str.length(); i++) {
            switch (toupper(str[i])) {
                case 'F': oldPoint = vec3(curPoint); curPoint += (curDirection);
                    lines.insert(lines.end(), {Line({
                        rotate(scale(translate(mat4(1), oldPoint), 0.1f * vec3(5.0 - sqrt(pow(oldPoint.x, 2.0) + pow(oldPoint.y, 2.0) + pow(oldPoint.z, 2.0)))), radians(30.0f), oldDirection) ,
                        rotate(scale(translate(mat4(1), curPoint), 0.1f * vec3(5.0 - sqrt(pow(curPoint.x, 2.0) + pow(curPoint.y, 2.0) + pow(curPoint.z, 2.0)))), radians(30.0f), curDirection)
                    })});; break;
                case '+': oldDirection = curDirection; curDirection = rotate(curDirection, angle, VEC3::Z); break;
                case '-': oldDirection = curDirection; curDirection = rotate(curDirection, -angle, VEC3::Z); break;
                case '/': oldDirection = curDirection; curDirection = rotate(curDirection, angle, VEC3::X); break;
                case '\\': oldDirection = curDirection; curDirection = rotate(curDirection, -angle, VEC3::X); break;
                case '!': oldDirection = curDirection; curDirection = MATH::randomPointOnSphere(35, 180); break;
                case '&': oldDirection = curDirection; curDirection = MATH::randomPointOnSphere(35, -180); break;
                case '[': memory.push(curPoint); memory.push(curDirection); break;
                case ']':
                    if (memory.empty()) break;
                    oldDirection = curDirection; curDirection = memory.top(); memory.pop();
                    curPoint = memory.top(); memory.pop(); break;
            }
        }


        std::vector<vec3> points = {vec3(-1,0,0), vec3(0,0,1), vec3(1,0,0), vec3(0,0,-1)};
        for (auto l : lines)
        {
            Mesh* _ = l.wrapMeshAround(&points, true, false);
            tree->vertices.insert(tree->vertices.end(), _->vertices.begin(), _->vertices.end());
        }
        tree->nrOfVerts = tree->vertices.size() / VA_POSITION_NORMAL.getVertSize();


        VertexBuffer::uploadSingleMesh(tree);
    }

    void debugRender(Gizmos* gizmos);

    void render(float time);
};

