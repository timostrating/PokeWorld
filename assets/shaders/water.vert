#version 300 es

layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;

out vec4 v_pos;
out vec2 v_uv;

const float tiling = 6.0;

void main() {
    v_uv = a_pos.xy * tiling;
    v_pos = MVP * vec4(a_pos, 1.0);
    gl_Position = v_pos;
}