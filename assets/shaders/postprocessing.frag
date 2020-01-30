#version 300 es
precision mediump float;

out vec4 outputColor;

in vec2 v_uv;

uniform sampler2D u_texture;
uniform sampler2D u_depth;
uniform vec2 u_resolution;

const mat3 sy = mat3(
    3.0, 10.0, 3.0,
    0.0, 0.0, 0.0,
    -3.0, -10.0, -3.0
);
const mat3 sx = mat3(
    3.0, 0.0, -3.0,
    10.0, 0.0, -10.0,
    3.0, 0.0, -3.0
);
const vec2 size = vec2(1600.0, 1600.0);

void main()
{
    vec3 diffuse = texture(u_texture, v_uv.st).rgb;
    mat3 A;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            A[i][j] = length(texelFetch(u_depth, ivec2(vec2(gl_FragCoord) / u_resolution * size) + ivec2(i-1,j-1), 0 ).rgb);
        }
    }

    float gx = dot(sx[0], A[0]) + dot(sx[1], A[1]) + dot(sx[2], A[2]);
    float gy = dot(sy[0], A[0]) + dot(sy[1], A[1]) + dot(sy[2], A[2]);

    float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
    //            float gradientDirection = atan(gy/gx);

    g = smoothstep(0.0, 0.02, g);

    outputColor = vec4(mix(diffuse, diffuse * vec3(0.05), g), 1.);
}