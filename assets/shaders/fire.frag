out vec4 outputColor;

in vec3 v_pos;

uniform float u_time;

vec3 fire1 = vec3(255.0/255.0, 206.0/255.0,  96.0/255.0);
vec3 fire2 = vec3(243.0/255.0,  99.0/255.0,  1.0/255.0);

void main() {
    float d = sqrt(pow(v_pos.x, 2.0) + pow(v_pos.y +0.6, 2.0));
    d = clamp(d - 1.0, -0.7, 0.1) * 3.0;

    float n = noise(d + v_pos * 2.0 + u_time * vec3(0.7, -2.2, 0.5));
    n += 0.1*noise(d + v_pos * 5.0 + u_time * vec3(2.7, -5.2, 1.5));
    outputColor = vec4(mix(fire1, fire2, smoothstep(0.4, 0.601, n)), 1.0);
}