out vec4 outputColor;

uniform float u_time;

in vec3 v_pos;
in vec3 v_normal;


const vec3 tree1 = vec3( 68.0/255.0,  60.0/255.0,  39.0/255.0);
const vec3 tree2 = vec3( 21.0/255.0,  14.0/255.0,  13.0/255.0);

const vec3 grass1 = vec3(112.0/255.0, 139.0/255.0,  70.0/255.0);
const vec3 grass2 = vec3( 50.0/255.0,  86.0/255.0,  49.0/255.0) * 0.8;

void main() {
    outputColor = vec4(smoothAmbiend(u_time, v_normal) * mix(tree1, tree2, noise(v_pos * 3.0)), 1.0);
    outputColor.xyz = gammaCorrection(outputColor.xyz);
}