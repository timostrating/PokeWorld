#version 300 es
layout (location = 0) in vec3 a_pos;

out vec3 v_pos;

uniform mat4 MVP;

void main()
{
    v_pos = a_pos;
    gl_Position = MVP * vec4(a_pos, 1.0);
}