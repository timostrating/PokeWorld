#version 300 es
precision mediump float;

out vec4 outputColor;

in vec2 v_uv;
uniform sampler2D u_texture;

void main() {
    outputColor = texture(u_texture, v_uv);
}