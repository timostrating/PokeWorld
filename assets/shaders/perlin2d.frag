in vec3 v_pos;
uniform float u_time;

out vec4 outputColor;


vec2 randomGradient(float ix, float iy) {
    // Random float. No precomputed gradients mean this works for any number of grid coordinates
    float random = 2920.f * sin(ix * 21942.f + iy * 171324.f + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
    return vec2(cos(random), sin(random));
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(float ix, float iy, float x, float y) {
    // Get gradient from integer coordinates
    vec2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - ix;
    float dy = y - iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
float perlin(vec2 xy) {
    float x = xy[0];
    float y = xy[1];

    // Determine grid cell coordinates
    float x0 = floor(x);
    float x1 = x0 + 1.;
    float y0 = floor(y);
    float y1 = y0 + 1.;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - x0;
    float sy = y - y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = smoothstep(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = smoothstep(n0, n1, sx);

    value = smoothstep(ix0, ix1, sy);
    return value;
}

void main() {
    vec2 pos = v_pos.xy;
    outputColor = vec4(vec3(perlin(pos)), 1.0);
}
