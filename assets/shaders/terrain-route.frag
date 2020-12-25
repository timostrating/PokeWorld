out vec4 outputColor;

in vec3 v_pos;
in vec3 v_normal;


const vec3 color1 = vec3(108.0/255.0, 130.0/255.0, 169.0/255.0);
const vec3 color2 = vec3(100.0/255.0,  66.0/255.0, 101.0/255.0);
const vec3 color3 = vec3( 20.0/255.0,  15.0/255.0,  21.0/255.0);

const vec3 route1 = vec3(193.0/255.0, 166.0/255.0, 146.0/255.0);
const vec3 route2 = vec3(171.0/255.0, 130.0/255.0, 111.0/255.0);

const vec3 grass1 = vec3(112.0/255.0, 139.0/255.0,  70.0/255.0);
const vec3 grass2 = vec3( 50.0/255.0,  86.0/255.0,  49.0/255.0) * 0.8;

const vec3 rock1 = vec3(189.0/255.0, 110.0/255.0, 86.0/255.0);
const vec3 rock2 = vec3( 81.0/255.0,  17.0/255.0, 14.0/255.0);


uniform float u_time;


void main() {
    float a = pattern(v_pos * .125);
    float b = pattern(v_pos);
    float r = mix(0.5, mix(a, b, 0.4), 0.75);

    vec3 ground = mix(vec3(99.0/255.0, 57.0/255.0, 41.0/255.0) * 0.2, vec3(165.0/255.0, 123.0/255.0, 82.0/255.0) * 0.6, r);
    vec3 grass = mix(grass1, grass2, r);
    vec3 route = mix(route1, route2, r);

    outputColor.xyz = smoothAmbiend(u_time, v_normal) * mix(route, grass, clamp01(r+0.45));
    outputColor.xyz = gammaCorrection(outputColor.xyz);
    outputColor.w = 1.0;

    gl_FragDepth = gl_FragCoord.z - gl_FragCoord.w / (500.0) ;//- 7.0/(255.0*255.0); // postprocessing hack
}