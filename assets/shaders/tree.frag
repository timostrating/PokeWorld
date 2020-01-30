out vec4 outputColor;

uniform float u_time;

in vec3 v_pos;
in vec3 v_normal;


const vec3 rock1 = vec3(189.0/255.0, 110.0/255.0, 86.0/255.0);
const vec3 rock2 = vec3( 81.0/255.0,  17.0/255.0, 14.0/255.0);
const vec3 grass2 = vec3( 50.0/255.0,  86.0/255.0,  49.0/255.0);

void main() {

    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(1.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(0.0/255.0,   10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

    vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
    float NdotL1 = dot(normalize(v_normal), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

    outputColor.xyz = vec3(mix(0.5 * rock1, 0.5 * rock2, noise(v_pos * 3.0)));
    outputColor.xyz = ambiend * mix(outputColor.xyz, grass2, clamp(v_pos.y * 0.3 - 0.9, 0.0, 1.0));
    outputColor.w = 1.0;
}