#version 300 es

layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;

out vec3 v_pos;
out vec4 v_mvpPos;

void main() {
    v_pos = a_pos;
    v_mvpPos = MVP * vec4(a_pos, 1.0);
    gl_Position = v_mvpPos;
}