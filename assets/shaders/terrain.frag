#version 300 es
precision mediump float;

out vec4 outputColor;

uniform sampler2D tex0;

in vec3 v_pos;


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

vec3 mix3(vec3 a, vec3 b, vec3 c, float t) {
    if (t < 0.5)
    return mix(a, b, t*2.0);
    else
    return mix(b, c, (t-0.5)*2.0);
}

const vec3 color1 = vec3(108.0/255.0, 130.0/255.0, 169.0/255.0);
const vec3 color2 = vec3(100.0/255.0,  66.0/255.0, 101.0/255.0);
const vec3 color3 = vec3( 20.0/255.0,  15.0/255.0,  21.0/255.0);

const vec3 route1 = vec3(171.0/255.0, 130.0/255.0, 111.0/255.0);
const vec3 route2 = vec3(193.0/255.0, 166.0/255.0, 146.0/255.0);

const vec3 grass1 = vec3(112.0/255.0, 139.0/255.0,  70.0/255.0);
const vec3 grass2 = vec3( 50.0/255.0,  86.0/255.0,  49.0/255.0);

void main() {
    float v = noise(v_pos * 2.0);

    if (v_pos.y < 9.0)
        outputColor = vec4(mix3(color1, color2, color3, v), 1.0);
    else {
        if (noise(v_pos * .3) < 0.7)
            outputColor = vec4(mix(grass1, grass2, v), 1.0);
        else
            outputColor = vec4(mix(route1, route2, v), 1.0);
    }

}