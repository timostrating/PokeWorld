#version 300 es
precision mediump float;

out vec4 outputColor;

in vec3 TexCoords;

uniform float u_time;
//uniform samplerCube skybox;


float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}


const vec4 nightColor1 = vec4(  1.0/255.0,  65.0/255.0, 134.0/255.0, 1.0);
const vec4 nightColor2 = vec4(  0.0/255.0,   0.0/255.0,  36.0/255.0, 1.0);

const vec4 dayColor1 = vec4( 84.0/255.0, 172.0/255.0, 208.0/255.0, 1.0);
const vec4 dayColor2 = vec4( 42.0/255.0,  99.0/255.0, 142.0/255.0, 1.0);



void main() {
    float dayTime = abs(sin(u_time * 0.1)); // 0 equals night,  1 equals day
    vec4 night = vec4(1.0);
    vec4 day = vec4(1.0);

    float t = clamp(abs(TexCoords.y * 3.0), 0.0, 1.0);


    night = mix(nightColor1, nightColor2, t);
    float v = noise(TexCoords * 100.0); // stars
    if (v > 0.98)
        night = mix(nightColor1, vec4(v,v,v,1.0), t - dayTime);


    day = mix(dayColor1, dayColor2, t);


    outputColor = mix(night, day, dayTime);
}