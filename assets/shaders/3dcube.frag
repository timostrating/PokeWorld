#version 300 es
precision mediump float;

in vec3 fragmentColor;

out vec4 outputColor;


void main() {
    gl_FragColor = vec4(fragmentColor, 1.0);
}