#version 300 es

layout (location = 0) in vec3 a_pos;

uniform mat4 MVP;

out vec3 v_pos;
//out float gl_ClipDistance[1];

//const vec4 clipPlane = vec4(0.0, -1.0, 0.0, 0.15);

void main() {

    vec4 woldPosition = MVP * vec4(a_pos, 1.0);

//    gl_ClipDistance[0] = dot(woldPosition, clipPlane);

    v_pos = a_pos;
    gl_Position = woldPosition;
}