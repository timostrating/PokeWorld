out vec4 outputColor;

uniform float u_time;
uniform vec3 u_treepos;

in vec3 v_pos;
in vec3 v_normal;


const vec3 leaves1 = vec3( 38.0/255.0, 124.0/255.0,  44.0/255.0);
const vec3 leaves2 = vec3( 17.0/255.0,  77.0/255.0,  24.0/255.0);


void main() {
    float dcenter = sqrt(pow(v_pos.x - u_treepos.x, 2.0) + pow(v_pos.y - u_treepos.y - 6.0, 2.0) + pow(v_pos.z - u_treepos.z, 2.0)) * 0.45;
    outputColor.xyz = mix(leaves2, leaves1, mix(noise(u_treepos),noise(dcenter * v_pos * 0.1 + v_pos * 3.0), 0.3));
    dcenter -= 0.3 * noise(v_pos * 5.0 + vec3(sin(u_treepos.x + 20.0 * u_time)));

    if(1.0 - dcenter <= 0.0)
        gl_FragDepth = 1.0; // postprocessing hack
    else
        gl_FragDepth = gl_FragCoord.z; // postprocessing hack



    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(1.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(0.0/255.0,   10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

    vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
    float NdotL1 = dot(normalize(v_normal), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

    outputColor = vec4(ambiend * ambiend * outputColor.xyz, 1.0);
}