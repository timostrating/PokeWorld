#version 300 es
precision mediump float;

in vec4 v_pos;

out vec4 outputColor;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;
uniform sampler2D u_dudvTexture;

const vec4 color1 =     vec4(  6.0/255.0,  77.0/255.0, 173.0/255.0, 1.0);
const vec4 color2 =     vec4( 34.0/255.0, 120.0/255.0, 190.0/255.0, 1.0);
const vec4 colorDark =  vec4( 17.0/255.0,  33.0/255.0,  80.0/255.0, 1.0);
const vec4 colorLight = vec4(201.0/255.0, 216.0/255.0, 230.0/255.0, 1.0);

void main() {
    vec2 ndc = (v_pos.xy/v_pos.w)/2.0 + 0.5;

    vec4 reflection = texture(u_reflectionTexture, vec2(ndc.x, -ndc.y));
    vec4 refraction = texture(u_refractionTexture, vec2(ndc.x, ndc.y));
//    outputColor = refraction; // mix(refraction, reflection, 0.5);


    if (refraction.x > 0.6)
        outputColor = colorLight;
    else
        outputColor = mix(reflection, color1, 0.5);
}