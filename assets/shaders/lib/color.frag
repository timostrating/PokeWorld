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
    vec3 light_pos = vec3(sin(u_time) * 3.0, 5.0, cos(u_time) * 3.0);
    float NdotL1 = dot(normalize(v_normal), normalize(light_pos));

    outputColor =  vec4(mix(u_color, shadowColor(u_color), smoothstep(0.5, 0.4, NdotL1)), 1.0);
}