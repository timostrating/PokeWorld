#version 300 es

layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;


void main() {
    gl_Position = MVP * vec4(a_pos, 1.0);
}