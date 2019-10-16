#version 300 es

layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;

out vec4 v_color;

void main() {
    v_color = vec4(0.3 + (a_pos.x/25.), 0.1 + (a_pos.y/25.), 0.1 + (a_pos.z/25.), 1);
    gl_Position = MVP * vec4(a_pos, 1.0);
}