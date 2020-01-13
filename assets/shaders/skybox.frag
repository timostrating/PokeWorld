#version 300 es
precision mediump float;

out vec4 outputColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    outputColor = texture(skybox, TexCoords);
}