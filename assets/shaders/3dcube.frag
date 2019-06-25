#version 300 es
precision lowp float;

varying vec3 fragmentColor;

void main() {
    gl_FragColor = vec4(fragmentColor, 1.0);
}