//
// Created by sneeuwpop on 18-11-19.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "../util/interfaces/screen.h"
#include "../graphics/shader_program.h"
#include "../graphics/flying_camera.h"
#include "../util/input/keyboard.h"
#include "../util/splines/line.h"
#include "../graphics/frame_buffer.h"

using namespace glm;
using namespace MATH;

#define MULTILINE(...) #__VA_ARGS__

class EverydayScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        out vec3 v_pos;
        out mat4 v_MVP;

        void main() {
            v_pos = a_pos;
            v_MVP = MVP;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";


    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision highp float;
        #define PI 3.1415926535

        out vec4 outputColor;

        in vec3 v_pos;
        in mat4 v_MVP;
        uniform float u_time;

        const vec2 u_resolution = vec2(800.0, 800.0);

            // https://en.wikipedia.org/wiki/Rotation_matrix#General_rotations
            mat3 euler_to_mat3(vec3 euler) {
                float a = euler.x;  // yaw
                float b = euler.y;  // pitch
                float g = euler.z;  // roll
                return mat3(
                    cos(a)*cos(b), cos(a)*sin(b)*sin(g) - sin(a)*cos(g), cos(a)*sin(b)*cos(g) + sin(a)*sin(g),
                    sin(a)*cos(b), sin(a)*sin(b)*sin(g) + cos(a)*cos(g), sin(a)*sin(b)*cos(g) - cos(a)*sin(g),
                    -1.0*sin(b), cos(b)*sin(g), cos(b)*cos(g)
                );
            }

            mat4 euler_to_mat4(vec3 euler) {
                float a = euler.x;  // yaw
                float b = euler.y;  // pitch
                float g = euler.z;  // roll
                return mat4(
                    cos(a)*cos(b), cos(a)*sin(b)*sin(g) - sin(a)*cos(g), cos(a)*sin(b)*cos(g) + sin(a)*sin(g), 0.0,
                    sin(a)*cos(b), sin(a)*sin(b)*sin(g) + cos(a)*cos(g), sin(a)*sin(b)*cos(g) - cos(a)*sin(g), 0.0,
                    -1.0*sin(b), cos(b)*sin(g), cos(b)*cos(g), 0.0,
                    0.0, 0.0, 0.0, 1.0
                );
            }

        /* See: https://www.iquilezles.org/www/articles/smin/smin.htm. */
        float math_smoothmin(float a, float b, float c)
        {
          if (c != 0.0) {
            float h = max(c - abs(a - b), 0.0) / c;
            return min(a, b) - h * h * h * c * (1.0 / 6.0);
          }
          else {
            return min(a, b);
          }
        }

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

        float clamp01(float v) { return clamp(v, 0.0, 1.0); }



        const int MAX_MARCHING_STEPS = 255;
        const float MIN_DIST = 0.0;
        const float MAX_DIST = 100.0;
        const float EPSILON = 0.0001;

        // https://iquilezles.org/www/articles/distfunctions/distfunctions.htm

        float sphereSDF(vec3 p, float r) {
            return length(p) - r;
        }
        float boxSDF(vec3 p, vec3 r) {
            vec3 d = abs(p) - r;
            return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
        }
        float sdCone( vec3 p, vec2 c, float h )
        {
          float q = length(p.xz);
          return max(dot(c.xy,vec2(q,p.y)),-h-p.y);
        }
        float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
        {
          vec3 pa = p - a, ba = b - a;
          float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
          return length( pa - ba*h ) - r;
        }

        float dot2(vec2 v) { return dot(v,v); }

        float sdCappedCone( vec3 p, float h, float r1, float r2 )
        {
          vec2 q = vec2( length(p.xz), p.y );
          vec2 k1 = vec2(r2,h);
          vec2 k2 = vec2(r2-r1,2.0*h);
          vec2 ca = vec2(q.x-min(q.x,(q.y<0.0)?r1:r2), abs(q.y)-h);
          vec2 cb = q - k1 + k2*clamp( dot(k1-q,k2)/dot2(k2), 0.0, 1.0 );
          float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
          return s*sqrt( min(dot2(ca),dot2(cb)) );
        }

        float sdEllipsoid( vec3 p, vec3 r )
        {
          float k0 = length(p/r);
          float k1 = length(p/(r*r));
          return k0*(k0-1.0)/k1;
        }


        float intersectSDF(float distA, float distB) { return max(distA, distB); }
        float unionSDF(float distA, float distB) { return min(distA, distB); }
        float differenceSDF(float distA, float distB) { return max(distA, -distB); }

        vec2 sceneSDF(vec3 samplePoint) {
            // snowman body
            float s = sphereSDF(samplePoint, 0.2);
            s = math_smoothmin(s, sphereSDF(samplePoint - vec3(0.0, 0.3, 0.0), 0.15), 0.05);
            s = math_smoothmin(s, sphereSDF(samplePoint - vec3(0.0, 0.55, 0.0), 0.13), 0.05);

            // floor
            s = math_smoothmin(s, sdCone(samplePoint +  vec3(0.0, 0.15, 0.0), vec2(0.1,0.8),0.4), 0.05);

            // had
            float dark = sdCappedCone(samplePoint - vec3(0.0, 0.75, 0.0), 0.09, 0.10, 0.12);
            dark = math_smoothmin(dark, sdCappedCone(samplePoint - vec3(0.0, 0.65, 0.0), 0.01, 0.2, 0.2), 0.1);

            // eyes
            dark = min(dark, sdEllipsoid(samplePoint - vec3( 0.05, 0.6, 0.12), vec3(0.02)));
            dark = min(dark, sdEllipsoid(samplePoint - vec3(-0.05, 0.6, 0.12), vec3(0.02)));

            // mouth
            dark = min(dark, sdEllipsoid(samplePoint - vec3( 0.05, 0.52, 0.12), vec3(0.02)));
            dark = min(dark, sdEllipsoid(samplePoint - vec3( 0.02, 0.51, 0.12), vec3(0.02)));
            dark = min(dark, sdEllipsoid(samplePoint - vec3( 0.0 , 0.5, 0.15), vec3(0.02)));
            dark = min(dark, sdEllipsoid(samplePoint - vec3(-0.02, 0.51, 0.12), vec3(0.02)));
            dark = min(dark, sdEllipsoid(samplePoint - vec3(-0.05, 0.52, 0.12), vec3(0.02)));

            // hand right
            float brown = sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(0.15, 0.0, 0.0), vec3(0.25, 0.15 - 0.03, 0.0), 0.005);
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(0.25, 0.12, 0.0), vec3(0.3, 0.2 - 0.03, 0.0), 0.005), 0.01);
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(0.25, 0.12, 0.0), vec3(0.32, 0.17 - 0.03, 0.0), 0.005), 0.01);
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(0.25, 0.12, 0.0), vec3(0.27, 0.22 - 0.03, 0.0), 0.005), 0.01);

            // hand left
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(-0.15, 0.0, 0.0), vec3(-0.25, 0.15 - 0.03, 0.0), 0.005), 0.01);
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(-0.25, 0.12, 0.0), vec3(-0.3, 0.2 - 0.03, 0.0), 0.005), 0.01);
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(-0.25, 0.12, 0.0), vec3(-0.32, 0.17 - 0.03, 0.0), 0.005), 0.01);
            brown = math_smoothmin(brown, sdCapsule(samplePoint - vec3(0.0, 0.27, 0.0), vec3(-0.25, 0.12, 0.0), vec3(-0.27, 0.22 - 0.03, 0.0), 0.005), 0.01);


            if (s < dark && s < brown)
                return vec2(s, 0.0);
            if (brown < dark)
                return vec2(brown, -1.0);
            else
                return vec2(dark, 1.0);

        }

        vec3 estimateNormal(vec3 p) {
            return normalize(vec3(
                sceneSDF(vec3(p.x + EPSILON, p.y, p.z)).x - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)).x,
                sceneSDF(vec3(p.x, p.y + EPSILON, p.z)).x - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)).x,
                sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)).x - sceneSDF(vec3(p.x, p.y, p.z - EPSILON)).x
            ));
        }

        vec2 shortestDistanceToSurface(vec3 eye, vec3 marchingDirection, float start, float end) {
            float depth = start;
            for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
                vec2 dist = sceneSDF(eye + depth * marchingDirection);
                if (dist.x < EPSILON) {
                    return vec2(depth, dist.y);
                }
                depth += dist.x;
                if (depth >= end) {
                    return vec2(end, 0.0);
                }
            }
            return vec2(end, 0.0);
        }

        vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
            vec2 xy = fragCoord - size / 2.0;
            float z = size.y / tan(radians(fieldOfView) / 2.0);
            return normalize(vec3(xy, -z));
        }


        vec2 rotate2D(vec2 _st, float _angle){
            _st -= 0.5;
            _st =  mat2(cos(_angle),-sin(_angle),
                        sin(_angle),cos(_angle)) * _st;
            _st += 0.5;
            return _st;
        }

        vec2 tile(vec2 _st, float _zoom){
            _st *= _zoom;
            return fract(_st);
        }

        float box(vec2 _st, vec2 _size, float _smoothEdges){
            _size = vec2(0.5)-_size*0.5;
            vec2 aa = vec2(_smoothEdges*0.5);
            vec2 uv = smoothstep(_size,_size+aa,_st);
            uv *= smoothstep(_size,_size+aa,vec2(1.0)-_st);
            return uv.x*uv.y;
        }

        float haltonSequence(int base, int index){
            float f = 1.0;
            float result = 0.0;

            while (index > 0) {
                f = f / float(base);
                result += f * float(index % base);
                index = index / base;
            }
            return result;
        }

        // http://www.aclockworkberry.com/shader-derivative-functions/
        // https://www.ronja-tutorials.com/2019/11/29/fwidth.html
        float aaStep(float compValue, float gradient){
            float halfChange = fwidth(gradient) / 2.;

            float lowerEdge = compValue - halfChange;
            float upperEdge = compValue + halfChange;

            float stepped = (gradient - lowerEdge) / (upperEdge - lowerEdge);

            return stepped;
        }

        // https://link.springer.com/content/pdf/10.1007%2F3-540-60590-8_29.pdf
        vec2 tea(vec4 k) {
            int y=0;
            int z=0;
            int sum=0;
            int delta = 2654435769;
            for(int n=0; n<32; n++) {
                sum += delta;
                y += ((z<<4)+int(k[0])) ^ (z+sum) ^ ((z>>5)+int(k[1]));
                z += ((y<<4)+int(k[2])) ^ (y+sum) ^ ((y>>5)+int(k[3]));
            }

            return vec2(y,z);
        }

        // https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
        vec3 ACESFilm(vec3 x) {
            float a = 2.51;
            float b = 0.03;
            float c = 2.43;
            float d = 0.59;
            float e = 0.14;
            return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
        }

        // https://www.shadertoy.com/view/WdycW3
        // https://en.wikipedia.org/wiki/Planck%27s_law
        float B(float v, float T) {
            float h = 6.62607015e-34;
            float c = 299792458.0;
            float kB = 1.380649e-23;
            return ((2.0*h * v*v*v) / (c*c)) / (exp( (h*v) / (kB*T) ) -1.0);
        }

        // https://www.scientificbulletin.upb.ro/rev_docs_arhiva/full49129.pdf
        vec3 wavelength2RGB(float w) {
            if (w < 410.) return vec3(0.);
            if (w < 440.) return vec3(0.19 - 0.19 * (440. - w) / 30., 0., 1.);
            if (w < 490.) return vec3(0., 1. - (490. - w) / 50., 1.);
            if (w < 510.) return vec3(0., 1., (510. - w) / 20.);
            if (w < 580.) return vec3(1. - (580. -w)/70., 1., 0.);
            if (w < 640.) return vec3(1., (640. -w)/60., 0.);
            if (w < 700.) return vec3(1., 0., 0.);
            if (w < 780.) return vec3(0.35 + 0.65 * (780. - w)/80., 0., 0.);
            return vec3(0.);
        }

        float gaussian(float x, float alpha, float mu, float sigma1, float sigma2) {
          float squareRoot = (x - mu)/((x < mu) ? sigma1 : sigma2);
          return alpha * exp( -(squareRoot * squareRoot)/2. );
        }

        vec3 xyzFromWavelength(float wavelength) {
          float x = gaussian(wavelength,  1.056, 5998., 379., 310.)
                  + gaussian(wavelength,  0.362, 4420., 160., 267.)
                  + gaussian(wavelength, -0.065, 5011., 204., 262.);

          float y = gaussian(wavelength,  0.821, 5688., 469., 405.)
                  + gaussian(wavelength,  0.286, 5309., 163., 311.);

          float z = gaussian(wavelength,  1.217, 4370., 118., 360.)
                  + gaussian(wavelength,  0.681, 4590., 260., 138.);

            return vec3(x,y,z);
        }

    // https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
    vec3 temperature2RGBNaive(float T100) {
        float T = clamp(T100 / 100., 1000., 40000.);
        float r = 0.;
        if (T <= 66.) {
            r = 255.;
        } else {
            r = 329.698727446 * pow(T - 60., -0.1332047592);
        }

        float g = 0.;
        if (T <= 66.) {
            g = 99.4708025861 * log(T) - 161.1195681661;
            g = clamp(g, 0., 255.);
        } else {
            g = 288.1221695283 * pow(T - 60., -0.0755148492);
        }

        float b = 0.;
        if (T >= 66.) {
            b = 255.;
        } else {
            if (T <= 19.) {
                b = 0.;
            } else {
                b = 138.5177312231 * log(T - 10.) - 305.0447927307;
            }
        }

        return vec3(r/255., g/255., b/255.);
    }

    // https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
    vec3 temperature2RGB(float T) {
        float Tk = T / 100.;

        float r = min(255., 329.698727446 * pow(Tk - 60., -0.1332047592));
        float g = min(
            99.4708025861 * log(Tk) - 161.1195681661,
            288.1221695283 * pow(Tk - 60., -0.0755148492)
        );
        float b = 138.5177312231 * log(Tk - 10.) - 305.0447927307;

        return clamp(vec3(r,g,b) / 255., 0., 1.);
    }

    // from https://www.shadertoy.com/view/WtfXRj
    float hash2( vec2 p ) {return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453123);} //Pseudo-random
    float smoothNoise2d( in vec2 p) { //Bilinearly interpolated noise (4 samples)
        vec2 i = floor( p ); vec2 f = fract( p );
        vec2 u = f*f*(3.0-2.0*f);
        float a = hash2( i + vec2(0.0,0.0) );
        float b = hash2( i + vec2(1.0,0.0) );
        float c = hash2( i + vec2(0.0,1.0) );
        float d = hash2( i + vec2(1.0,1.0) );
        return float(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y)/4.;
    }

    // https://www.microsoft.com/en-us/research/wp-content/uploads/2007/10/tr-2007-141.pdf
    int leftRotate (int x, int c) {
        return (x << c) | (x >> (32-c));
    }
    vec3 whiteNoise3Md5(vec3 pos) {
        vec3 result = vec3(
            leftRotate(int(floor(abs(sin(pos.x)) * 4294967296.0)), 7),
            leftRotate(int(floor(abs(sin(pos.y)) * 4294967296.0)), 12),
            leftRotate(int(floor(abs(sin(pos.z)) * 4294967296.0)), 17)
        );
        return vec3(result / 4294967296.0);
    }

    float GPURnd (in vec4 state ) {
        const vec4 q = vec4(1225.0, 1585.0, 2457.0, 2098.0);
        const vec4 r = vec4(1112.0, 367.0, 92.0, 265.0);
        const vec4 a = vec4(3423.0, 2646.0, 1707.0, 1999.0);
        const vec4 m = vec4(4194287.0, 4194277.0, 4194191.0, 4194167.0);
        vec4 beta = floor(state / q);
        vec4 p = a*mod(state,q) - beta*r;
        beta = (sign(-p)+vec4(1)) * vec4(0.5) * m;
        state = (p+beta);
        return fract(dot(state/m,vec4(1,-1,1,-1)));
    }

    // https://www.bidouille.org/prog/plasma
    float oldSchoolPlasmaV(vec3 pos, float time) {
        float v = 0.;
        v += sin(pos.x * 10. + time/3.);
        v += sin(10. * (pos.x * sin(time / 4.) + pos.y * cos(time / 5.) + pos.z * cos(time / 6.)) + time/2.);
        float cx = pos.x + .5 * sin(time / 10.);
        float cy = pos.y + .5 * sin(time / 9.);
        float cz = pos.z + .5 * sin(time / 8.);
        v += sin(sqrt(100. * (cx*cx + cy+cy + cz*cz) + 1.) + time/2.);
        return v;
    }

    float smootherStep(float a0, float a1, float w) {
        return (a1 - a0) * (w * (w * 6.0 - 15.0) * w * w *w + 10.0) + a0;
    }

    float circle(vec2 uv, vec2 pos, float r, float blur) {
        float d = length(uv - pos);
        return smoothstep(r, r-blur, d);
    }

    float easeInOutQuad( float x )
    {
        x = clamp( x, 0., 1. );
        return x < 0.5 ? 2. * x * x : 1. - pow(-2. * x + 2., 2.) / 2.;
    }

    vec3 RGB2SMPTE170M(vec3 RGB) {
        return mat3(0.299, 0.587, 0.114, 0.711, -0.587, -0.114, -0.299, -0.587, 0.886) * RGB;
    }

    #define NOISE fbm
    #define NUM_NOISE_OCTAVES 5

    // Precision-adjusted variations of https://www.shadertoy.com/view/4djSRW
    float hash(float p) { p = fract(p * 0.011); p *= p + 7.5; p *= p + p; return fract(p); }
    float hash(vec2 p) {vec3 p3 = fract(vec3(p.xyx) * 0.13); p3 += dot(p3, p3.yzx + 3.333); return fract((p3.x + p3.y) * p3.z); }

    float valueNoise(vec3 x) {
        const vec3 step = vec3(110, 241, 171);

        vec3 i = floor(x);
        vec3 f = fract(x);

        // For performance, compute the base input to a 1D hash from the integer part of the argument and the
        // incremental change to the 1D based on the 3D -> 1D wrapping
        float n = dot(i, step);

        vec3 u = f * f * (3.0 - 2.0 * f);
        return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                       mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
                   mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                       mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
    }

    float fbm(vec3 x) {
        float v = 0.0;
        float a = 0.5;
        vec3 shift = vec3(100);
        for (int i = 0; i < NUM_NOISE_OCTAVES; ++i) {
            v += a * valueNoise(x);
            x = x * 2.0 + shift;
            a *= 0.5;
        }
        return v;
    }

    // https://github.com/victor-shepardson/webgl-gabor-noise/blob/master/gnoise.glsl#L46
    const float pp_epsilon = (1./289.);
    float nextRand(float u){//rng
        u = fract((u*34.0+1.0)*u+pp_epsilon);
        return fract((7.*289./288.)*u);
    }

    //Excerpt from https://www.shadertoy.com/view/4lfXDM
    //By nmz, https://www.shadertoy.com/user/nmz
    //texh is the function that generates the Hatching TEXture
    //p seems to be the uv for hatching, str seems to be the strength
    float texh(in vec2 p, in float str)
    {
        p*= .7; //Scale p
        float rz= 1.;
        for (int i=0;i<10;i++)
        {
            float g = hash(100. *vec2(0.025,.5)*p);
            g = smoothstep(0.-str*0.1,2.3-str*0.1,g);
            rz = min(1.-g,rz);
            p.xy = p.yx;
            p += .07;
            p *= 1.2;
            if (float(i) > str)break;
        }
        return rz*1.05;
    }

    float leaves(vec2 uv) {
//        float s = noise(vec3(uv * 10., 0.));

        return texh(uv, abs(sin(u_time)));
    }

    vec3 sepia(in vec3 col)
    {
        return vec3(dot(col, vec3(0.393,0.769,0.189)),
                    dot(col, vec3(0.349,0.686,0.168)),
                    dot(col, vec3(0.272,0.534,0.131)));
    }

    vec3 marble_color(float x)
    {
        vec3 clr = vec3(0);
        x = sqrt(x+1.0)*.7071;
        clr.g = .30 + .8*x;
        x=sqrt(x);
        clr.r = .30 + .6*x;
        clr.b = .60 + .4*x;
        return (clr);
    }

    float perlin_turbulence(vec3 p) {
        float t = 0.;
        float scale = 1.;
        for (int i = 0; i < 5; ++i) {
            t += abs(noise(p / scale) * scale);
            scale /= 2.;
        }
        return t;
    }

    vec3 marble(vec3 pnt)
    {
        float y = 0.;
        y = pnt.y + 3.0*perlin_turbulence(pnt);
        y = sin(y*PI);
        return marble_color(y);
    }

    float tri(in float x){return abs(fract(x)-.5);}
    vec3 tri3(in vec3 p){return vec3( tri(p.z+tri(p.y*1.)), tri(p.z+tri(p.x*1.)), tri(p.y+tri(p.x*1.)));}

    float triNoise3d(in vec3 p, in float spd)
    {
        float z=1.4;
        float rz = 0.;
        vec3 bp = p;
        for (float i=0.; i<=2.; i++ )
        {
            vec3 dg = tri3(bp*2.);
            p += (dg+u_time*spd);

            bp *= 1.8;
            z *= 1.5;
            p *= 1.2;

            rz+= (tri(p.z+tri(p.x+tri(p.y))))/z;
            bp += 0.14;
        }
        return rz;
    }

    float brush_effect(float dist, float h_avg, float h_var)
    {
        float h = max(0.0,1.0-10.0*abs(dist));
        h *= h;
        h *= h;
        return (h_avg+h_var*h) * smoothstep(-0.01, 0.002, dist);
    }

vec3 random3r( vec3 p ) {
    return fract(sin(
        vec3(dot(p,vec3(127.1,227.1,311.7)),
            dot(p,vec3(817.2,217.2,146.2)),
            dot(p,vec3(269.5,269.5,183.3)))
        )*43758.5453);
}

/* The fractal_noise functions are all exactly the same except for the input type. */
float fractal_noise(vec3 p, float octaves, float roughness)
{
  float fscale = 1.0;
  float amp = 1.0;
  float maxamp = 0.0;
  float sum = 0.0;
  octaves = clamp(octaves, 0.0, 16.0);
  int n = int(octaves);
  for (int i = 0; i <= n; i++) {
    float t = noise(fscale * p);
    sum += t * amp;
    maxamp += amp;
    amp *= clamp(roughness, 0.0, 1.0);
    fscale *= 2.0;
  }
  float rmd = octaves - floor(octaves);
  if (rmd != 0.0) {
    float t = noise(fscale * p);
    float sum2 = sum + t * amp;
    sum /= maxamp;
    sum2 /= maxamp + amp;
    return (1.0 - rmd) * sum + rmd * sum2;
  }
  else {
    return sum / maxamp;
  }
}

float minkowski_distance(vec3 a, vec3 b, float exponent) {
    return pow(pow(abs(a.x - b.x), exponent) + pow(abs(a.y - b.y), exponent) + pow(abs(a.z - b.z), exponent), 1.0 / exponent);
}

vec3 mainVoronoi() {

    vec3 pos = v_pos;
    pos += vec3(1.);
    pos *= 2.;

    vec3 i_st = floor(pos);
    vec3 f_st = fract(pos);

    vec3 color = vec3(0.);

    float m_dist = 1.;  // minimum distance
    vec3 m_value = vec3(0);
    vec3 mr = vec3(0);
    vec3 point = vec3(0.);

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            for (int z= -1; z <= 1; z++) {
                vec3 neighbor = vec3(float(x), float(y), float(z));
                point = random3r(i_st + neighbor);

                // Vector between the pixel and the point
                vec3 diff = neighbor + point - f_st;

                // Distance to the point
                float dist = length(diff);
//                float dist = minkowski_distance(point - f_st, neighbor, 0.35 );

                // Keep the closer distance
                if (dist < m_dist) {
                    m_dist = dist;
                    m_value = point;
                    mr = diff;
                }

            }
        }
    }

    float md = 8.0;
    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            for (int z= -1; z <= 1; z++) {
                vec3 neighbor = vec3(float(x), float(y), float(z));
                point = random3r(i_st + neighbor);

                // Vector between the pixel and the point
                vec3 diff = neighbor + point - f_st;

                if( dot(mr-diff,mr-diff)>0.00001 )
                md = min( md, dot( 0.5*(mr+diff), normalize(diff-mr)));
            }
        }
    }

//    return vec3(m_dist);

    // Draw the min distance (distance field)
    color = m_value;

//    vec3 rotated = euler_to_mat3(color) * v_pos;
//    float distortion = 5. * (sin(u_time * 0.3) * 0.5 + 0.5);
//    vec3 line = vec3( 1.0 + pow(sin( rotated.y * 400.0 + distortion * (fractal_noise(v_pos, 5.2, 0.8) *2.0 -1.0) ), 4.0) );
//    line += vec3(abs(sin(rotated.y * 200.0)));


//    if (noise(v_pos * 7.0) > 0.3)
//        line = vec3(1.0);
//
//    if (md < 0.03)
//        line = vec3(1.0);

    return color;
}

uint Squirrel3( uint n, uint seed) {
    uint BIT_NOISE1 = 0xb5297a4du;  // 0b0110'1000'1110'0011'0001'1101'1010'0100
    uint BIT_NOISE2 = 0x68e31da4u;  // 0b1011'0101'0010'1001'0111'1010'0100'1101
    uint BIT_NOISE3 = 0x1b56c4e9u;  // 0b0001'1011'0101'0110'1100'0100'1110'1001

    n *= BIT_NOISE1;
    n += seed;
    n ^= n >> 8u;
    n += BIT_NOISE2;
    n ^= n << 8u;
    n *= BIT_NOISE3;
    n ^= n >> 8u;
    return n;
}

int xorshift(int value) {
    // Xorshift*32
    // Based on George Marsaglia's work: http://www.jstatsoft.org/v08/i14/paper
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    return value;
}

// from: https://www.shadertoy.com/view/ltfGDs
vec3 nmzHash33(vec3 q)
{
    uvec3 p = uvec3(ivec3(q));
    p = p*uvec3(374761393U, 1103515245U, 668265263U) + p.zxy + p.yzx;
    p = p.yzx*(p.zxy^(p >> 3U));
    return vec3(p^(p >> 16U))*(1.0/vec3(0xffffffffU));
}

// https://en.wikipedia.org/wiki/Relative_luminance
float rgb2RelativeLuminance(vec3 color)
{
  vec3 factors = vec3(0.2126, 0.7152, 0.0722);
  return dot(color.rgb, factors);
}


float abstri(in float x){return abs(fract(x)-.5);}
vec3 stars(in vec3 p)
{
    vec3 c = vec3(0.);
    float res = 800.0*0.8;

    p.x += (abstri(p.z*50.)+abstri(p.y*50.)) * (0.02 + 0.02 * sin(u_time*0.3));
    p.y += (abstri(p.z*50.)+abstri(p.x*50.)) * (0.02 + 0.02 * sin(u_time*0.3));
    p.z += (abstri(p.x*50.)+abstri(p.y*50.)) * (0.02 + 0.02 * sin(u_time*0.3));


	for (float i=0.;i<3.;i++)
    {
        vec3 q = fract(p*(.15*res))-0.5;
        vec3 id = floor(p*(.15*res));
        vec2 rn = nmzHash33(id).xy;
        float c2 = 1.-smoothstep(0.,.6,length(q));
        c2 *= step(rn.x,.005+i*i*0.005);
        c += c2*(mix(vec3(1.0, 0.0, 0.0),vec3(0.0, 0.0, 1.0),rn.y)*0.5+0.5);
        p *= 1.7;
    }
    return c*c*4.7;
}

const mat3 m = mat3( 0.80,  0.60, 0.0,
                    -0.60,  0.00, 0.80,
                    -0.60,  0.00, 0.80 );

float sin_noise( in vec3 p )
{
	return sin(p.x)*sin(p.y)*sin(p.z);
}

float fbm4( vec3 p )
{
    float f = 0.0;
    f += 0.5000*sin_noise( p ); p = m*p*2.02;
    f += 0.2500*sin_noise( p ); p = m*p*2.03;
    f += 0.1250*sin_noise( p ); p = m*p*2.01;
    f += 0.0625*sin_noise( p );
    return f/0.9375;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));

    float dotLN = dot(L, N);
    float dotRV = dot(R, V);

    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    }

    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 light1Pos = vec3(4.0 * sin(u_time), 2.0, 4.0 * cos(u_time));
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);

    color += phongContribForLight(k_d, k_s, alpha, p, eye, light1Pos, light1Intensity);
    return color;
}



vec3 mainPoke() {
    vec3 dir = rayDirection(45.0, vec2(800.0), gl_FragCoord.xy);
    vec3 eye = euler_to_mat3(vec3(0., sin(u_time * 0.25) * 0.05, 0.)) * vec3(0.0, 0.2, 5.0);
    vec2 dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);


    if (dist.x > MAX_DIST - EPSILON) { // Didn't hit anything
        vec3 col = vec3(0.45,0.65,1.1)*0.3 + (dir.y)*0.5;
        vec3 fogcol = vec3(0.1,0.2,0.5)*0.25;
        col = mix( col, fogcol, exp2(-8.0*max(dir.y + 0.05,0.0)) );


        return (fbm(dir * 300.0 + vec3(0.0, u_time, 0.0)) > 0.7)? vec3(1.0, 1.0 , 0.9) : col;
    }

    vec3 p = eye + dist.x * dir;   // closest point on the surface
    p -= 0.08 * fbm(p * 100.0);

    vec3 snow = (fbm(dir * 200.0 + vec3(0.0, u_time *2.0, 0.0)) > 0.7)? vec3(1.0, 1.0 , 0.9) : vec3(0.0);

    if (dist.y <= 0.0 + EPSILON || fbm(p * 10.0) > 0.6) {
        if (dist.y <= -1.0 + EPSILON) {
            return vec3(87.0/255.0, 65.0/255.0, 47.0/255.0) * 0.3;
        }
        vec3 color = phongIllumination(vec3(0.9), vec3(0.9), vec3(0.9), 10.0, p, eye);
        return color - 0.1 * fbm(p * 100.0) + snow;
    }

    return vec3(20.0/255.0) + snow;
}


#define xor(p) xorshift(int(floor(p * 100.0)))
#define i2f(seed) abs(fract(float(seed) / 3141.592653))

        void main(void) {

//            vec3 color = 1e13 * vec3(B(1.14e15, 273.15 + v_pos.x  * 3000.));
//            vec3 color = wavelength2RGB(500. + v_pos.y * 20.) + 0.2 * noise(v_pos * 10.);
//            vec3 color = 100 * xyzFromWavelength(600. + v_pos.y * 170.);
//            vec3 color = temperature2RGB(abs(100. + (v_pos.y+1.) * 5000.));
//            vec3 color = vec3(smoothNoise2d(2. * v_pos.xy), 0, smoothNoise2d(2. * v_pos.zy));
//            vec3 color = vec3(float(leftrotate(1234, 2)));
//            vec3 color = vec3(1.0 - GPURnd(vec4(v_pos * 100., sin(u_time) * 100.)));

//            vec3 color = mix(vec3(50.0/255.0, 191.0/255.0, 231.0/255.0), vec3(222.0/255.0, 238.0/255.0, 241.0/255.0), smoothstep( 0.4, 0.6, fbm(v_pos + vec3(0., 0., u_time * 0.1))));

            float a = smoothstep(1.0, sqrt(3), length(v_pos.xyz));

            vec2 pos = v_pos.xy;
            if (abs(pos.x) > 0.99)
                pos.x = v_pos.z;

//            vec3 color = vec3(0., 0., brush_effect(distance(v_pos, 0.2 * vec3(noise(v_pos * 10.0))) - 1.0, 0.1, sin(u_time) *0.5 + 0.5));
//            vec3 color = vec3(fractal_noise(v_pos, 4.0, sin(u_time*0.3)));
//            vec3 color = mainVoronoi();

            vec3 color = mainPoke();

            outputColor = vec4( color, 1.0 );
        }

)glsl";

    FlyingCamera camera = FlyingCamera();

//    ShaderProgram shader = ShaderProgram::fromAssetFiles("shaders/voronoi.vert", "shaders/perlin2d.frag");
    ShaderProgram shader = ShaderProgram(vertSource, fragSource);

//    Texture texture = Texture::fromAssetFile("textures/tur.jpg");

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());

//    ShaderProgram postProcessingShader = ShaderProgram::fromAssetFiles("shaders/postprocessing.vert", "shaders/postprocessing.frag");
    GLint MVP, u_resolution;
//    FrameBuffer screenFbo = FrameBuffer(800, 800);


public:
    EverydayScreen()
    {
        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
        VertexBuffer::uploadSingleMesh(sphere);
//        texture.bind(0, shader, "u_texture");

//        MVP = postProcessingShader.uniformLocation("MVP");
//        u_resolution = postProcessingShader.uniformLocation("u_resolution");
//        screenFbo.addColorTexture();
//        screenFbo.addDepthTexture();
//        screenFbo.unbind();
    }


    void setup(GLFWwindow* window)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
        // TODO: Fill optional fields of the io structure later.
        // TODO: Load TTF/OTF fonts if you don't want to use the default font.

        const char* glsl_version = "#version 300 es"; // TODO: We should not hardcoded that here

        // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32 and imgui_impl_dx11)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    double time = 0;
    bool anyKeyPressed = false;

    void render(double deltaTime) {
        time += deltaTime;
        shader.use();
        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(0/255.0, 0/255.0, 0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(5, 5, 5);
            camera.lookAt(vec3(0.0, -0.2, 0.0));
            camera.Camera::update();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

//        screenFbo.bind();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(rotate(glm::scale(Camera::main->combined, vec3(2,2,2)), radians(static_cast<float>(time) * 15.0f), VEC3::Y))[0][0]);
            cube->render();

//        screenFbo.unbind();



        //////////////////////////////////////////////////////////////////////////////////////////////// POST PROCESSING

//        camera.saveState();
//        camera.position = vec3(0, 0, 1 / tan(radians(camera.fov * 0.5)));
//        camera.lookAt(vec3(0, 0, 0));
//        camera.Camera::update();
////
//        fireShader.use();
//        glUniformMatrix4fv(fireShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined)[0][0]);
//        glUniform1f(fireShader.uniformLocation("u_time"), time);
////        glUniform2f(u_resolution, camera.width, camera.height);
////        screenFbo.colorTexture->bind(0, postProcessingShader, "u_texture");
////        screenFbo.depthTexture->bind(1, postProcessingShader, "u_depth");
//        quad->render();
//        camera.restoreState();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////// GUI
//        shader.use();
//        VertexBuffer::bindDefault();
//
//        // Feed inputs to dear imgui, start new frame
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        ImGui::Begin("LSystem pattern");
//        ImGui::SliderFloat("m", &m, -10, 10);
//        ImGui::SliderFloat("n1", &n1, -10, 10);
//        ImGui::SliderFloat("n2", &n2, -10, 10);
//        ImGui::SliderFloat("n3", &n3, -10, 10);
//        ImGui::End();
//
//        // Render dear imgui into screen
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};