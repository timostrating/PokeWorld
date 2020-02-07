
#define PI 3.1415926535
#define PHI 1.61803398874989484820459


/////////////////////////////////////////////////////////////////////////////////////////////////////////////// NOISE 3D

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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////// NOISE 2D

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////// FBM

const int FBM_OCTAVES = 3;
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

float pattern( vec3 p )
{
    vec3 q = vec3( fbm( p + vec3(0.0, 0.0, 0.0) ), fbm( p + vec3(5.2,1.3,2.5) ), fbm(vec3(3.4,2.1,7.1) ) );

    return fbm( p + 1.5 * q ) * 0.7;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////// SHORT HANDS

vec3 gradient(mat4 gradient, float t) {
    vec3 value = vec3(1.0, 0.0, 1.0);

    if (gradient[0][3] >= t) // before gradient
    return vec3(gradient[0][0], gradient[0][1], gradient[0][2]);

    for (int i=0; i<3; i++) {
        if (gradient[i+1][3] >= t) { // mix i, i+1
            vec3 a = vec3(gradient[i+0][0], gradient[i+0][1], gradient[i+0][2]);
            vec3 b = vec3(gradient[i+1][0], gradient[i+1][1], gradient[i+1][2]);
            return mix(a,b, (t - gradient[i+0][3]) / (gradient[i+1][3] - gradient[i+0][3])); }}

    return vec3(gradient[3][0], gradient[3][1], gradient[3][2]);    // after gradients
}

float clamp01(float v) { return clamp(v, 0.0, 1.0); }



vec3 mix3(vec3 a, vec3 b, vec3 c, float t) {
    if (t < 0.5)
    return mix(a, b, t*2.0);
    else
    return mix(b, c, (t-0.5)*2.0);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float gold_noise(in vec2 xy, in float seed){
    return fract(tan(distance(xy * PHI, xy)*seed)*xy.x);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////// COLORS

vec3 smoothAmbiend(float time, vec3 normal) {
    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(40.0/255.0,  75.0/255.0, 144.0/255.0), abs(sin(time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(15.0/255.0,   30.0/255.0,  56.0/255.0), abs(sin(time *0.5)));

    vec3 light_pos = vec3(sin(time), cos(time), 0.0);
    float NdotL1 = dot(normalize(normal), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

    return ambiend;
}