#version 300 es
precision mediump float;

out vec4 outputColor;

in vec4 v_color;

void main() {
    outputColor = v_color;
}