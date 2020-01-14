#version 300 es
precision mediump float;

in vec4 v_pos;
in vec2 v_uv;

out vec4 outputColor;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;
uniform sampler2D u_dudvTexture;

uniform float u_time;

const vec4 color1 =     vec4(  6.0/255.0,  77.0/255.0, 173.0/255.0, 1.0);
const vec4 color2 =     vec4( 34.0/255.0, 120.0/255.0, 190.0/255.0, 1.0);
const vec4 colorDark =  vec4( 17.0/255.0,  33.0/255.0,  80.0/255.0, 1.0);
const vec4 colorLight = vec4(201.0/255.0, 216.0/255.0, 230.0/255.0, 1.0);

const float waveStrength = 0.02;


void main() {
    vec2 ndc = (v_pos.xy/v_pos.w)/2.0 + 0.5;
    vec2 reflectUV = vec2(ndc.x, -ndc.y);
    vec2 refractUV = vec2(ndc.x, ndc.y);

    float distanceCompensation = pow(clamp(1. + ndc.y * -2.0, 0.0, 1.0), 2.0); // todo improve
    vec2 distortion = (texture(u_dudvTexture, vec2(v_uv.x + u_time * 0.08 + sin(u_time) * 0.03, v_uv.y)).rg * 2.0 - 1.0) * waveStrength * distanceCompensation;
    refractUV += distortion;
    refractUV = clamp(refractUV, 0.001, 0.999);

    vec4 reflection = texture(u_reflectionTexture, reflectUV);
    vec4 refraction = texture(u_refractionTexture, refractUV);

    if (refraction.x > 0.64)
        outputColor = colorLight;
    else
        outputColor = mix(reflection, color1, 0.5);
}