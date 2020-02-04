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
    SharedMesh leaves = SharedMesh(new Mesh(0,0, VA_POSITION_NORMAL));
    mat4 transform;
    vec3 position;
    ShaderProgram treeShader = ShaderProgram::fromAssetFiles("shaders/tree.vert", "shaders/tree.frag");
    ShaderProgram leavesShader = ShaderProgram::fromAssetFiles("shaders/leaves.vert", "shaders/leaves.frag");
    GLuint treeMVP, tree_time, leavesMVP, leaves_time, leaves_pos;
    const float angle = radians(15.f);

    std::vector<Line> lines;

    static const int EXAMPLE_SIZE = 10;
    static const std::string EXAMPLES[EXAMPLE_SIZE];

    std::string usedPattern;

    void addLeave(mat4 leave)
    {
        vec3 a1 = transform * leave * vec4(-0.5 + random(-0.2, 0.2),  0.5 + random(-0.2, 0.2), 0 + random(-0.4, 0.4),  1.0);
        vec3 a2 = transform * leave * vec4(-0.5 + random(-0.2, 0.2), -0.5 + random(-0.2, 0.2), 0 + random(-0.4, 0.4),  1.0);
        vec3 b1 = transform * leave * vec4( 0.5 + random(-0.2, 0.2),  0.5 + random(-0.2, 0.2), 0 + random(-0.4, 0.4),  1.0);
        vec3 b2 = transform * leave * vec4( 0.5 + random(-0.2, 0.2), -0.5 + random(-0.2, 0.2), 0 + random(-0.4, 0.4),  1.0);

        vec3 n1 = cross(a2-a1, b1-a1),  n2 = cross(a2-b1, b2-b1);
        leaves->vertices.insert(leaves->vertices.end(), {a1.x,a1.y,a1.z, n1.x,n1.y,n1.z, a2.x,a2.y,a2.z, n1.x,n1.y,n1.z, b1.x,b1.y,b1.z, n1.x,n1.y,n1.z,   b1.x,b1.y,b1.z, n2.x,n2.y,n2.z, a2.x,a2.y,a2.z, n2.x,n2.y,n2.z, b2.x,b2.y,b2.z, n2.x,n2.y,n2.z,});
        leaves->nrOfVerts += 6; // TODO not hardcode this here
    }


    Tree(vec3 pos, std::string pattern = "F[-x[+&x]!]") : transform(
            scale(rotate(rotate(rotate(
                translate(mat4(1), pos),
                    radians(random(0.0f,15.0f)),  VEC3::X),
                    radians(random(0.0f,360.0f)), VEC3::Y),
                    radians(random(0.0f,15.0f)),  VEC3::Z),
            randomVec3(0.9, 1.1))), usedPattern(pattern), position(transform * vec4(0, 1.0, 0, 1))
    {
        position.y = random(-0.1, 2);
        treeMVP = treeShader.uniformLocation("MVP");
        tree_time = treeShader.uniformLocation("u_time");
        leavesMVP = leavesShader.uniformLocation("MVP");
        leaves_time = leavesShader.uniformLocation("u_time");
        leaves_pos = leavesShader.uniformLocation("u_treepos");

        LSystem lSystem = LSystem("X");
        lSystem.addPattern('X', pattern);

        vec3 curPoint = VEC3::ZERO, oldPoint = VEC3::ZERO;
        vec3 curDirection = VEC3::Y, oldDirection = VEC3::Y;


        std::stack<vec3> memory = std::stack<vec3>();
        float maxSize = random(5.0, 7.0);

        lSystem.applyNtimes(3);
        std::string str = lSystem.getStr();
        for (int i = 0; i < str.length(); i++) {
            switch (toupper(str[i])) {
                case 'F': oldPoint = vec3(curPoint); curPoint += (curDirection);
                    lines.insert(lines.end(), {Line({
                        rotate(scale(translate(mat4(1), oldPoint), 0.1f * vec3(clamp(maxSize - sqrt(pow(oldPoint.x, 2.0) + pow(oldPoint.y, 2.0) + pow(oldPoint.z, 2.0)), 0.0, 999.0))), radians(30.0f), oldDirection) ,
                        rotate(scale(translate(mat4(1), curPoint), 0.1f * vec3(clamp(maxSize - sqrt(pow(curPoint.x, 2.0) + pow(curPoint.y, 2.0) + pow(curPoint.z, 2.0)), 0.0, 999.0))), radians(30.0f), curDirection)
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

            vec3 pos = l.getPointPosition(1);
            if (pos.y > 2.0)
                addLeave(rotate(scale(translate(mat4(1), pos + vec3(random(-0.1, 0.1), 0.1, random(-0.1, 0.1))), randomVec3(3.5, 6.0)),radians(random(0, 50)), randomVec3(-1,1))); // random leave
        }
        tree->nrOfVerts = tree->vertices.size() / VA_POSITION_NORMAL.getVertSize();

        addLeave(scale(rotate(translate(mat4(1), vec3(0.0, 6.0, 0.0)), radians(90.0f), VEC3::X), vec3(5))); // leave midway to prevent strange top down look


        VertexBuffer::uploadSingleMesh(tree);
        VertexBuffer::uploadSingleMesh(leaves);
//        colorPickerData = new ColorPickerData({tree, transform});
    }

    void debugRender(Gizmos* gizmos);

    void render(float time);
};

