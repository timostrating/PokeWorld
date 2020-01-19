//
// Created by sneeuwpop on 17-01-20.
//

#include <imgui.h>
#include <glad/glad.h>
#include "../util/interfaces/screen.h"
#include "../graphics/flying_camera.h"
#include "../graphics/shader_program.h"
#include "../util/input/keyboard.h"
#include "../graphics/frame_buffer.h"


class ColorTestScreen : public Screen
{

    constexpr static char vertSource[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;
        // layout (location = 1) in vec3 a_normal; // fake normal

        uniform mat4 MVP;

        out vec3 v_normal;

        void main() {
            v_normal = a_pos; // fake normal
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    constexpr static char fragSource[] = R"glsl(#version 300 es
        precision mediump float;

        in vec3 v_normal;
        out vec4 outputColor;

        const float u_time = 0.0;
        uniform vec3 u_color;

        // avarage color changes in shadows in the anime from 10+ samples each
        const vec3 avgShadowFactorDay   = vec3(0.8239, 0.7126, 0.7905);
        const vec3 avgShadowFactorNight = vec3(0.6099, 0.6013, 0.6711);
        const vec3 avgShadowCastFactor  = vec3(0.5259, 0.5282, 0.6595);

        vec3 shadowColor(vec3 diffuseColor) {
            return diffuseColor * avgShadowFactorDay;
        }

        void main() {
            vec3 light_pos = vec3(sin(u_time) * 3.0, 5.0, cos(u_time) * 3.0);
            float NdotL1 = dot(normalize(v_normal), normalize(light_pos));

            outputColor =  vec4(mix(u_color, shadowColor(u_color), smoothstep(0.0, -0.05, NdotL1)), 1.0);
        })glsl";




    constexpr static char vertSource2[] = R"glsl(#version 300 es
        layout (location = 0) in vec3 a_pos;

        uniform mat4 MVP;

        out vec2 v_uv;

        void main() {
            v_uv = 1.0 - (a_pos.xy + 1.0) / 2.0;
            v_uv *= -1.0;
            gl_Position = MVP * vec4(a_pos, 1.0);
        })glsl";

    constexpr static char fragSource2[] = R"glsl(#version 300 es
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
        const vec2 size = vec2(800.0, 800.0);

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

            g = smoothstep(0.0, 0.1, g);

            outputColor = vec4(mix(diffuse, vec3(0.2), g), 1.);
        } )glsl";

    FlyingCamera camera = FlyingCamera();

    ShaderProgram shader = ShaderProgram(vertSource, fragSource);
    ShaderProgram flatTextureShader = ShaderProgram(vertSource2, fragSource2);

//    ShaderProgram flatTextureShader = ShaderProgram::fromAssetFiles("shaders/lib/flat_texture.vert", "shaders/lib/flat_texture.frag");
//    Texture texture = Texture::testCheckerboard();

    Gizmos gizmos;
    SharedMesh cube = SharedMesh(Mesh::cube());
    SharedMesh quad = SharedMesh(Mesh::quad());
    SharedMesh sphere = SharedMesh(Mesh::sphere());

    FrameBuffer fbo = FrameBuffer(800, 800);


public:
    ColorTestScreen()
    {
        fbo.addColorTexture();
        fbo.addDepthTexture();
        fbo.unbind();

        shader.use();
        VertexBuffer::uploadSingleMesh(cube);
        VertexBuffer::uploadSingleMesh(quad);
        VertexBuffer::uploadSingleMesh(sphere);
//        texture.bind(0, shader, "u_texture");
    }

    double time = 0;
    bool anyKeyPressed = false;
    float a = 0.01, b = 0.6;

    void render(double deltaTime) {
        time += deltaTime;
//        shader.use();
//        glUniform1f(shader.uniformLocation("u_time"), time);

        glClearColor(61.0/255.0, 130.0/255.0, 184.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (anyKeyPressed) {
            camera.update(deltaTime);
            camera.debugDraw();
        } else {
            if (INPUT::KEYBOARD::pressed(GLFW_KEY_TAB))
                anyKeyPressed = true;
            camera.position = vec3(0, 2, 5 + cos(time*0.6));
            camera.lookAt(vec3(2, 1, 0));
            camera.Camera::update();
        }

//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_O)) a += 0.01;
//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_I)) a -= 0.01;
//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_L)) b += 0.01;
//        if (INPUT::KEYBOARD::pressed(GLFW_KEY_K)) b -= 0.01;

//        std::cout << a << ", " << b << "\n";
        /////////////////////////////////////////////////////////////////////////////////////////////////////////// TEST

        fbo.bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined)[0][0]);
            glUniform3f(shader.uniformLocation("u_color"), 198.0/255.0, 130.0/255.0, 150.0/255.0);
            sphere->render();

            glUniform3f(shader.uniformLocation("u_color"), 91.0/255.0, 107.0/255.0, 154.0/255.0);
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * translate(mat4(1), vec3(5, 0, -2)))[0][0]);
            sphere->render();
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * translate(mat4(1), vec3(4, 1, -4)))[0][0]);
            sphere->render();
            glUniformMatrix4fv(shader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * translate(mat4(1), vec3(3, 0, -4)))[0][0]);
            sphere->render();
        fbo.unbind();

        camera.saveState();
        camera.position = vec3(0, 0, 1 / tan(radians(camera.fov * 0.5)));
        camera.lookAt(vec3(0, 0, 0));
        camera.Camera::update();

        flatTextureShader.use();
        glUniformMatrix4fv(flatTextureShader.uniformLocation("MVP"), 1, GL_FALSE, &(camera.combined * scale(mat4(1), vec3(camera.width / static_cast<float>(camera.height), 1.0, 1.0)))[0][0]);
        glUniform1f(flatTextureShader.uniformLocation("a"), a);
        glUniform1f(flatTextureShader.uniformLocation("b"), b);
        glUniform2f(flatTextureShader.uniformLocation("u_resolution"), camera.width, camera.height);
        fbo.colorTexture->bind(0, flatTextureShader, "u_texture");
        fbo.depthTexture->bind(1, flatTextureShader, "u_depth");
        quad->render();
        camera.restoreState();
    }

    void resize(int width, int height)
    {
        camera.resize(width, height);
    }
};