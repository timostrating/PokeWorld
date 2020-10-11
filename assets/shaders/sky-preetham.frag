in vec3 v_pos;
uniform float u_time;

out vec4 outputColor;

#define M_PI_2 (2. * 3.14159265358979)
#define M_PI (3.14159265358979)

// https://en.wikipedia.org/wiki/CIE_1931_color_space#Construction_of_the_CIE_XYZ_color_space_from_the_Wright%E2%80%93Guild_data
vec3 XYZ_2_rgb(vec3 XYZ) {
    return mat3(0.41847, -0.15866, -0.082835,
                -0.091169, 0.25243, 0.015708,
                0.00092090, -0.0025498, 0.17860) * XYZ;
}
vec3 XYZ_to_rgb(float X, float Y, float Z) { return XYZ_2_rgb(vec3(X,Y,Z)); }

// https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space
vec3 xyY_to_XYZ(float x, float y, float Y) {
    float X = (Y/y)*x;
    float Z = (Y/y)*(1.-x-y);
    return vec3(X,Y,Z);
}
vec3 xyY_2_XYZ(vec3 xyY) {return xyY_to_XYZ(xyY[0],xyY[1],xyY[2]);}


// preetham sky paper: https://www2.cs.duke.edu/courses/cps124/spring08/assign/07_papers/p91-preetham.pdf
// Denoted in the paper as Yz
float sky_kittler_luminance_function(float theta, float gamma) {
    return ((0.91 + 10.* exp(-3.*gamma) + 0.45*pow(cos(gamma), 2.)) * (1. - exp(-0.32 / cos(theta)))) /
        ((0.91 + 10.* exp(-3.*theta) + 0.45*pow(cos(theta), 2.)) * (1. - exp(-0.32)));
    return theta;
}

// blender source code
// https://github.com/blender/blender/blob/master/source/blender/gpu/shaders/material/gpu_shader_material_tex_sky.glsl#L22
float sky_perez_function(float A, float B, float C, float D, float E, float theta, float gamma)
{
    return (1.0 + A * exp(B / cos(theta))) * (1.0 + C * exp(D * gamma) + E * pow(cos(gamma), 2.0));
}


float sky_angle_between(float thetav, float phiv, float theta, float phi)
{
    float cospsi = sin(thetav) * sin(theta) * cos(phi - phiv) + cos(thetav) * cos(theta);

    if (cospsi > 1.0) {
        return 0.0;
    }
    if (cospsi < -1.0) {
        return M_PI;
    }

    return acos(cospsi);
}

vec3 sky_spherical_coordinates(vec3 dir)
{
    return vec3(M_PI_2 - atan(dir.z, length(dir.xy)), atan(dir.x, dir.y), 0.);
}

void main() {
    vec3 sperical = sky_spherical_coordinates(v_pos.xzy);
    float theta = sperical[0];
    float phi = sperical[1];

    vec3 sun_angles = vec3(sin(u_time*0.5), cos(u_time*0.5), 0.);
    float suntheta = sun_angles[0];
    float sunphi = sun_angles[1];

    float gamma = sky_angle_between(theta, phi, suntheta, sunphi);

    /* clamp theta to horizon */
    theta = min(theta, M_PI_2 - 0.001);


    float T = sin(u_time)*5.+5.; // [0-10] range     // This paper says only between 1.9 > T < 10 is realistic http://wscg.zcu.cz/wscg2007/Papers_2007/short/E59-full.pdf

    float AY =  0.1787*T + -1.4563;
    float BY = -0.3554*T +  0.4275;
    float CY = -0.0227*T +  5.3251;
    float DY =  0.1206*T + -2.5771;
    float EY = -0.0670*T +  0.3703;

    float Ax = -0.0193*T + -0.2592;
    float Bx = -0.0665*T +  0.0008;
    float Cx = -0.0004*T +  0.2125;
    float Dx = -0.0641*T + -0.8989;
    float Ex = -0.0033*T +  0.0452;

    float Ay = -0.0167*T + -0.2608;
    float By = -0.0950*T +  0.0092;
    float Cy = -0.0079*T +  0.2102;
    float Dy = -0.0441*T + -1.6537;
    float Ey = -0.0109*T +  0.0529;

    float Y = sky_perez_function(AY, BY, CY, DY, EY, theta, gamma);
    float x = sky_perez_function(Ax, Bx, Cx, Dx, Ex, theta, gamma);
    float y = sky_perez_function(Ay, By, Cy, Dy, Ey, theta, gamma);

    outputColor = vec4(XYZ_2_rgb(xyY_to_XYZ(x,y,Y)), 1.0);
}
