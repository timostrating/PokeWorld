#version 300 es
precision mediump float;

out vec4 outputColor;

uniform vec4 u_color;

void main() {
    outputColor = u_color;
}