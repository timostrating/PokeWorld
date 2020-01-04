#version 300 es
precision mediump float;

out vec4 outputColor;

uniform sampler2D tex0;

in vec3 v_pos;
in vec3 v_color;

void main() {
    outputColor = texture(tex0, v_pos.xy * 0.1);
}