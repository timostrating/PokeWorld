#version 300 es
layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;

out vec3 v_pos;
out mat4 v_MVP;

void main() {
    v_pos = a_pos;
    v_MVP = MVP;
    gl_Position = MVP * vec4(a_pos, 1.0);
}