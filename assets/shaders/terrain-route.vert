#version 300 es

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 MVP;

out vec3 v_pos;
out vec3 v_normal;

void main() {

    vec4 woldPosition = MVP * vec4(a_pos, 1.0);

    v_pos = a_pos;
    v_pos.y = 1.0;
    v_normal = vec3(0.0, 1.0, 0.0);
    gl_Position = woldPosition;
}