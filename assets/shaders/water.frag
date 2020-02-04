in vec3 v_pos;
in vec4 v_mvpPos;

out vec4 outputColor;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;

uniform float u_time;

const vec4 colorDark =  vec4( 17.0/255.0,  33.0/255.0,  80.0/255.0, 1.0); // shadows
const vec4 color1 =     vec4( 34.0/255.0, 120.0/255.0, 190.0/255.0, 1.0);
const vec4 color2 =     vec4(  6.0/255.0,  77.0/255.0, 173.0/255.0, 1.0);
const vec4 colorLight = vec4(201.0/255.0, 216.0/255.0, 230.0/255.0, 1.0); // refraction

const float waveStrength = 0.02;
const float tiling = 6.0;


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

    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(10.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(10.0/255.0,  10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

    vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
    float NdotL1 = dot(normalize(vec3(0.0, 1.0, 0.0)), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));


    if (v_pos.y >= 0.99 && refraction.x > 0.64) // refraction
        outputColor = colorLight;
    else
        outputColor = mix(reflection * color1, (refraction + 0.34) * color1, 0.5);

    if (v_pos.y < 0.99) { // sides
        outputColor = mix(mix(vec4(ambiend, 1.0) * color2 * 0.5, (refraction*(1.0-abs(cos(u_time *0.5)))*0.3) + color1 * 0.5, 0.5) * 0.8, outputColor, clamp01(0.25-abs(v_pos.y-0.95)));
        outputColor.w = 0.97 - clamp((1.0 - v_mvpPos.w/15.0), 0.0, 0.02);
    }

    outputColor.xyz = ambiend * outputColor.xyz;
}