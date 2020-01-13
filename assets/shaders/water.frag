#version 300 es
precision mediump float;

in vec4 v_pos;

out vec4 outputColor;

//uniform sampler2D u_reflectionTexture;
uniform sampler2D u_depth;

void main() {
    vec2 ndc = (v_pos.xy/v_pos.w)/2.0 + 0.5;

    vec4 reflection = texture(u_depth, vec2(ndc.x, -ndc.y));
    outputColor = reflection; //mix(vec4(0.0, 0.0, 1.0, 1.0), reflection, 0.5);
}