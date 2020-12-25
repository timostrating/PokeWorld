in vec3 v_pos;
uniform float u_time;

out vec4 outputColor;


vec3 random3r( vec3 p ) {
    return fract(sin(
        vec3(dot(p,vec3(127.1,227.1,311.7)),
            dot(p,vec3(817.2,217.2,146.2)),
            dot(p,vec3(269.5,269.5,183.3)))
        )*43758.5453);
}

void main() {
    vec3 pos = v_pos;
    pos += vec3(1.);
    pos *= 2.;

    vec3 i_st = floor(pos);
    vec3 f_st = fract(pos);

    vec3 color = vec3(0.);

    float m_dist = 1.;  // minimum distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            for (int z= -1; z <= 1; z++) {
                // Neighbor place in the grid
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random position from current + neighbor place in the grid
                vec3 point = random3r(i_st + neighbor);

                // Animate the point
//                point = 0.5 + 0.5*sin(6.2831*point);

                // Vector between the pixel and the point
                vec3 diff = neighbor + point - f_st;

                // Distance to the point
                float dist = length(diff);

                // Keep the closer distance
                m_dist = min(m_dist, dist);
            }
        }
    }

    // Draw the min distance (distance field)
    color += mix(vec3(166./255.,133./255.,104./255.),vec3(64./255.,29./255.,22./255.),smoothstep(0.5, 1.0, m_dist));

    outputColor = vec4(color, 1.0);
}
