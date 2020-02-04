#version 300 es

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 MVP;

out vec3 v_pos;
out vec3 v_normal;


void main() {

    v_pos = a_pos;
    v_normal = v_pos * vec3(0.02, 1.0, 0.02);
    gl_Position = MVP * vec4(a_pos, 1.0);
}