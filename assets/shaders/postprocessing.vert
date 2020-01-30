#version 300 es
layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;

out vec2 v_uv;

void main() {
    v_uv = 1.0 - (a_pos.xy + 1.0) / 2.0;
    v_uv *= -1.0;
    gl_Position = MVP * vec4(a_pos, 1.0);
}