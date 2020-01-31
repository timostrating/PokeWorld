in vec3 v_normal;
out vec4 outputColor;

uniform float u_time;
uniform vec3 u_color;

// avarage color changes in shadows in the anime from 10+ samples each
const vec3 avgShadowFactorDay   = vec3(0.8239, 0.7126, 0.7905);
const vec3 avgShadowFactorNight = vec3(0.6099, 0.6013, 0.6711);
const vec3 avgShadowCastFactor  = vec3(0.5259, 0.5282, 0.6595);

vec3 shadowColor(vec3 diffuseColor) { return diffuseColor * avgShadowFactorDay; }

void main() {
    vec3 highlight = mix(vec3(245.0/255.0, 230.0/255.0, 221.0/255.0), vec3(1.0/255.0,  65.0/255.0, 134.0/255.0), abs(sin(u_time *0.5)));
    vec3 shadows =   mix(vec3(157.0/255.0, 142.0/255.0, 134.0/255.0), vec3(0.0/255.0,   10.0/255.0,  36.0/255.0), abs(sin(u_time *0.5)));

    vec3 light_pos = vec3(sin(u_time), cos(u_time), 0.0);
    float NdotL1 = dot(normalize(v_normal), normalize(light_pos));
    vec3 ambiend = mix(highlight, shadows, smoothstep(0.0, -0.20, NdotL1));

    outputColor =  vec4(ambiend * mix(u_color, shadowColor(u_color), smoothstep(0.5, 0.4, NdotL1)), 1.0);
}