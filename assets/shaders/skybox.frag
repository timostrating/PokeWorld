out vec4 outputColor;

in vec3 v_pos;

uniform float u_time;

const vec3 nightColor1 = vec3(  1.0/255.0,  35.0/255.0, 84.0/255.0);
const vec3 nightColor2 = vec3(  0.0/255.0,   0.0/255.0,  36.0/255.0);

const vec3 dayColor1 = vec3( 84.0/255.0, 172.0/255.0, 208.0/255.0);
const vec3 dayColor2 = vec3( 42.0/255.0,  99.0/255.0, 142.0/255.0);

float patterntime(vec3 p, float time)
{
    vec3 q = vec3(sin(time)*100.0, 1.0, sin(time * 0.5)*100.0) + vec3( fbm( p + vec3(0.0) ), fbm( p + vec3(5.2, 1.3, 2.5) ), fbm( vec3(3.4,2.1,7.1) ) );

    return fbm( p + 1.5 * q ) * 0.7;
}

void main() {
    float dayTime = abs(cos(u_time * 0.5)); // 0 equals night,  1 equals day
    float t = clamp01(abs(v_pos.y * 2.0));


    // NIGHT
    vec3 night = vec3(1.0);
    night = mix(nightColor1, nightColor2, t);
    float v = noise(v_pos * 100.0); // stars
    if (v > 0.98)
        night = mix(night, vec3(v), clamp01(t - dayTime));


    // DAY
    vec3 day = vec3(1.0);
    day = mix(dayColor1, dayColor2, t);


    outputColor = vec4(mix(night, day, dayTime*dayTime), 1.0); // mix day and night


    float f = patterntime(v_pos * vec3(5, 20, 5), u_time * 0.01); // clouds
    f = clamp01(f - 0.45) * abs(cos(u_time * 0.1));
    vec3 cloudColor = mix(vec3(8.0/255.0,27.0/255.0,78.0/255.0), vec3(1.0), dayTime * dayTime);
    outputColor.xyz = mix(outputColor.xyz, cloudColor, f);

    outputColor.xyz += vec3(0.0, abs(sin(v_pos.x*38910.0))/255.0, abs(sin(v_pos.z*40000.0))/255.0); // anti ditter
}