out vec4 outputColor;

uniform float u_time;

in vec3 v_pos;
in vec3 v_normal;


const vec3 tree1 = vec3( 68.0/255.0,  60.0/255.0,  39.0/255.0);
const vec3 tree2 = vec3( 21.0/255.0,  14.0/255.0,  13.0/255.0);

void main() {

    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(1.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(0.0/255.0,   10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

    vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
    float NdotL1 = dot(normalize(v_normal), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

    outputColor.xyz = ambiend * vec3(mix(tree1, tree2, noise(v_pos * 3.0))) ;
    outputColor.w = 1.0;
}