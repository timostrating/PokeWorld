#version 300 es
precision mediump float;

out vec4 outputColor;

uniform vec3 u_color;

void main() {
    outputColor = vec4(u_color, 1.0);
}