#version 300 es

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

uniform mat4 MVP;

out vec3 fragmentColor;


void main() {
    gl_Position = MVP * vec4(a_pos, 1.0);
    fragmentColor = a_color;
}