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

uniform mat4 u_gradient;

vec3 gradient(float t) {
    vec3 value = vec3(1.0, 0.0, 1.0);

    if (u_gradient[0][3] >= t) // before gradient
        return vec3(u_gradient[0][0], u_gradient[0][1], u_gradient[0][2]);

    for (int i=0; i<3; i++) {
        if (u_gradient[i+1][3] >= t) { // mix i, i+1
            vec3 a = vec3(u_gradient[i+0][0], u_gradient[i+0][1], u_gradient[i+0][2]);
            vec3 b = vec3(u_gradient[i+1][0], u_gradient[i+1][1], u_gradient[i+1][2]);
            return mix(a,b, (t - u_gradient[i+0][3]) / (u_gradient[i+1][3] - u_gradient[i+0][3])); }}

    return vec3(u_gradient[3][0], u_gradient[3][1], u_gradient[3][2]);    // after gradients
}

float clamp01(float v) { return clamp(v, 0.0, 1.0); }


const int FBM_OCTAVES = 7;
const float H = 1.5;

// http://iquilezles.org/www/articles/fbm/fbm.htm
float fbm(vec3 x)
{
    float G = exp2(-H);
    float f = 1.0;
    float a = 1.0;
    float t = 0.0;
    for( int i=0; i<FBM_OCTAVES; i++ )
    {
        t += a*noise(f*x);
        f *= 2.0;
        a *= G;
    }
    return t;
}

float pattern( in vec3 p )
{
    vec3 q = vec3( fbm( p + vec3(0.0, 0.0, 0.0) ), fbm( p + vec3(5.2,1.3,2.5) ), fbm(vec3(3.4,2.1,7.1) ) );

    return fbm( p + 1.5 * q ) * 0.7;
}

void main() {
    float r = pattern(v_pos);
    float r2 = pattern(v_pos * 5.0);

    outputColor = vec4(gradient(mix(r, r2, 0.2)), 1.0);

}