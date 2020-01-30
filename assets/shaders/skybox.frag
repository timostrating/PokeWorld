out vec4 outputColor;

in vec3 v_pos;

uniform float u_time;
//uniform samplerCube skybox;

const vec4 nightColor1 = vec4(  1.0/255.0,  65.0/255.0, 134.0/255.0, 1.0);
const vec4 nightColor2 = vec4(  0.0/255.0,   0.0/255.0,  36.0/255.0, 1.0);

const vec4 dayColor1 = vec4( 84.0/255.0, 172.0/255.0, 208.0/255.0, 1.0);
const vec4 dayColor2 = vec4( 42.0/255.0,  99.0/255.0, 142.0/255.0, 1.0);

void main() {
    float dayTime = 0.0; //abs(sin(u_time * 0.3)); // 0 equals night,  1 equals day
    vec4 night = vec4(1.0);
    vec4 day = vec4(1.0);

    float t = clamp(abs(v_pos.y * 3.0), 0.0, 1.0);


    night = mix(nightColor1, nightColor2, t);
    float v = noise(v_pos * 100.0); // stars
    if (v > 0.98)
        night = mix(night, vec4(v,v,v,1.0), clamp01(t - dayTime));


    day = mix(dayColor1, dayColor2, t);
    outputColor = mix(night, day, dayTime);
}