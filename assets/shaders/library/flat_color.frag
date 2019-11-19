#version 300 es
precision mediump float;

out vec4 outputColor;

uniform vec3 color;

void main() {
    outputColor = vec4(color, 1.0);
}