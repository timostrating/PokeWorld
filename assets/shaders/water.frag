#version 300 es
precision mediump float;

in vec3 v_pos;
in vec4 v_mvpPos;

out vec4 outputColor;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;

uniform float u_time;

const vec4 color1 =     vec4(  6.0/255.0,  77.0/255.0, 173.0/255.0, 1.0);
const vec4 color2 =     vec4( 34.0/255.0, 120.0/255.0, 190.0/255.0, 1.0);
const vec4 colorDark =  vec4( 17.0/255.0,  33.0/255.0,  80.0/255.0, 1.0);
const vec4 colorLight = vec4(201.0/255.0, 216.0/255.0, 230.0/255.0, 1.0);

const float waveStrength = 0.02;
const float tiling = 6.0;

// https://thebookofshaders.com/11/
// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
    vec2(12.9898,78.233)))
    * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
//     u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
    (c - a)* u.y * (1.0 - u.x) +
    (d - b) * u.x * u.y;
}

void main() {
    vec2 ndc = (v_mvpPos.xy/v_mvpPos.w)/2.0 + 0.5;
    vec2 reflectUV = vec2(ndc.x, -ndc.y);
    vec2 refractUV = vec2(ndc.x, ndc.y);

    float distanceCompensation = 0.0;
    if(v_pos.y >= 0.99)
        distanceCompensation = clamp(1.0 - (v_mvpPos.w * 0.015), 0.0, 1.0); // todo improve

    vec2 distortion = vec2(noise(v_pos.xz * 100.0 + u_time * 10.0), noise(v_pos.xz * 99.0 + u_time * 10.0)) * waveStrength * distanceCompensation;
    refractUV += distortion;
    refractUV = clamp(refractUV, 0.001, 0.999);

    vec4 reflection = texture(u_reflectionTexture, reflectUV);
    vec4 refraction = texture(u_refractionTexture, refractUV);

    if (v_pos.y >= 0.99 && refraction.x > 0.64)
        outputColor = colorLight;
    else
        outputColor = mix(reflection, (refraction + 0.34) * color1, 0.5);

    if (v_pos.y < 0.99)
        outputColor = mix(mix(reflection, refraction * color1, 0.5), outputColor, clamp(v_pos.y, 0.0, 1.0));



    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(10.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(10.0/255.0,  10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

    vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
    float NdotL1 = dot(normalize(vec3(0.0, 1.0, 0.0)), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

    outputColor.xyz = ambiend * outputColor.xyz;
    outputColor.w = 1.0;
}