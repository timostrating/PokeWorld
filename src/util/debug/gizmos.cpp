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
    "uniform vec3 from;"
    "uniform vec3 to;"

    "void main() {"
        "gl_Position = MVP * vec4((a_pos.x == 0.)? from : to, 1.0);"
    "}";

char fragSource[] = "#version 300 es\n"
    "precision mediump float;"
    "uniform vec4 color;"
    "out vec4 outputColor;"
    "void main() {"
        "outputColor = color;"
    "}";


Gizmos::Gizmos() : shaderProgram(ShaderProgram(vertSource, fragSource))
{
    mesh = SharedMesh(new Mesh(2,0, false));
    mesh->vertices.insert(mesh->vertices.begin(), {0,0,0, 1,1,1});
    mesh->renderMode = GL_LINE_STRIP;

    VertexBuffer::uploadSingleMesh(mesh);

    mvpId   = glGetUniformLocation(shaderProgram.getId(), "MVP");
    fromId  = glGetUniformLocation(shaderProgram.getId(), "from");
    toId    = glGetUniformLocation(shaderProgram.getId(), "to");
    colorId = glGetUniformLocation(shaderProgram.getId(), "color");
}

void Gizmos::drawLine(const vec3 &from, const vec3 &to, const vec4 &color)
{
    shaderProgram.use();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &Camera::main->combined[0][0]);
    glUniform3f(fromId, from.x, from.y, from.z);
    glUniform3f(toId, to.x, to.y, to.z);
    glUniform4f(colorId, color.x, color.y, color.z, color.w);
    mesh->render();
}

