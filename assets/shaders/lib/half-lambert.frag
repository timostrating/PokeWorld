out vec4 outputColor;

in vec3 v_pos;

uniform float u_time;

// copied from http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
void main() {
    vec3 SurfaceColor = vec3(0.5);         //objects color
    vec3 LightColor = hsv2rgb(vec3(sin(u_time *0.5)*0.5+0.5, 0.5, sin(u_time * 0.25)*0.25+0.75));           //lights color * intensity
    float LightAttenuation = 0.95;      //value of light at point (shadow/falloff)
    vec3 lightDirection = vec3(sin(u_time*0.5), 1.0, cos(u_time*0.5));

    float wrapValue = 1.0; //abs(sin(u_time*0.2))*0.5 + 0.5;

    float NdotL = max(0.0,dot(v_pos,lightDirection));
    float HalfLambertDiffuse = pow(NdotL * wrapValue + (1.-wrapValue),2.) * SurfaceColor;
    vec3 finalColor = HalfLambertDiffuse * LightAttenuation * LightColor;

    outputColor = vec4(finalColor + vec3(0.05), 1.0);

    outputColor.xyz += vec3(abs(sin(v_pos.x*4873.38))/255.0, abs(sin(v_pos.x*3819.12))/255.0, abs(sin(v_pos.z*5841.73))/255.0); // anti ditter
}
