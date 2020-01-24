#version 300 es

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 MVP;

out vec3 v_pos;
out vec3 v_normal;
//out float gl_ClipDistance[1];

//const vec4 clipPlane = vec4(0.0, -1.0, 0.0, 0.15);

void main() {

    vec4 woldPosition = MVP * vec4(a_pos, 1.0);

//    gl_ClipDistance[0] = dot(woldPosition, clipPlane);

    v_pos = a_pos;
    v_normal = a_normal;
    gl_Position = woldPosition;
}