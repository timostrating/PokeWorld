//
// Created by sneeuwpop on 30-09-19.
//

#include <iostream>
#include "gizmos.h"
#include "../../graphics/vertex_buffer.h"
#include "../../graphics/mesh.h"
#include "../../graphics/camera.h"

char vertSource[] = "#version 300 es\n"
    "layout (location = 0) in vec3 a_pos;"

    "uniform mat4 MVP;"
    "uniform vec3 u_from;"
    "uniform vec3 u_to;"

    "void main() {"
        "gl_Position = MVP * vec4((a_pos.x == 0.)? u_from : u_to, 1.0);"
    "}";

char fragSource[] = "#version 300 es\n"
    "precision mediump float;"
    "uniform vec4 u_color;"
    "out vec4 outputColor;"
    "void main() {"
        "outputColor = u_color;"
    "}";


Gizmos::Gizmos() : shaderProgram(ShaderProgram(vertSource, fragSource))
{
    mesh = SharedMesh(new Mesh(2,2));
    mesh->vertices.insert(mesh->vertices.begin(), {0,0,0, 1,1,1});
    mesh->indicies.insert(mesh->indicies.begin(), {0,1});
    mesh->renderMode = GL_LINES;

    VertexBuffer::uploadSingleMesh(mesh);

    mvpId   = shaderProgram.uniformLocation("MVP");
    fromId  = shaderProgram.uniformLocation("u_from");
    toId    = shaderProgram.uniformLocation("u_to");
    colorId = shaderProgram.uniformLocation("u_color");
}

void Gizmos::drawLine(const vec3 &from, const vec3 &to, const vec4 &color)
{
    shaderProgram.use();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &Camera::main->combined[0][0]);
    glUniform3f(fromId, from.x, from.y, from.z);
    glUniform3f(toId, to.x, to.y, to.z);
    glUniform4f(colorId, color.r, color.g, color.b, color.a);
    mesh->render();
}

void Gizmos::drawCube(const vec3 &pos, const float size, const vec4 &color) { drawBox(pos, vec3(size), color); }
void Gizmos::drawBox(const vec3 &pos, const vec3 size, const vec4 &color)
{
    shaderProgram.use();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &Camera::main->combined[0][0]);
    glUniform4f(colorId, color.x, color.y, color.z, color.w);

    const static vec3 data[] = {
            glm::vec3(-1,-1, 1), glm::vec3(1,-1, 1),
            glm::vec3(-1,-1,-1), glm::vec3(1,-1,-1),
            glm::vec3(-1, 1, 1), glm::vec3(1, 1, 1),
            glm::vec3(-1, 1,-1), glm::vec3(1, 1,-1),

            glm::vec3(-1,-1, 1), glm::vec3(-1,-1,-1),
            glm::vec3( 1,-1, 1), glm::vec3( 1,-1,-1),
            glm::vec3(-1, 1, 1), glm::vec3(-1, 1,-1),
            glm::vec3( 1, 1, 1), glm::vec3( 1, 1,-1),

            glm::vec3(-1,-1, 1), glm::vec3(-1, 1, 1),
            glm::vec3( 1,-1, 1), glm::vec3( 1, 1, 1),
            glm::vec3(-1,-1,-1), glm::vec3(-1, 1,-1),
            glm::vec3( 1,-1,-1), glm::vec3( 1, 1,-1),
    };
    for (int i=0; i<24; i+=2) {
        glUniform3f(fromId, (size.x * data[i  ].x) + pos.x,  (size.y * data[i  ].y) + pos.y,  (size.z * data[i  ].z) + pos.z);
        glUniform3f(toId,   (size.x * data[i+1].x) + pos.x,  (size.y * data[i+1].y) + pos.y,  (size.z * data[i+1].z) + pos.z);
        mesh->render();
    }
}

