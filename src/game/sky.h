//
// Created by sneeuwpop on 10-01-20.
//

#pragma once


#include "../util/interfaces/system.h"
#include "../graphics/shader_program.h"
#include "../graphics/cubemap.h"
#include "../graphics/mesh.h"
#include "../util/interfaces/game_object.h"
#include "../util/math/math.h"

class SkySystem : public System
{

public:
    float rotation = 0;
    void update(float deltatime) { rotation += deltatime * 2;}
};



class Sky : public GameObject
{
    mat4 transform = scale(translate(mat4(1.0f), vec3(0, -70, 0)), 400.0f * VEC3::ONE);

    ShaderProgram skyShader = ShaderProgram::fromAssetFiles("shaders/skybox.vert", "shaders/skybox.frag");
    std::vector<std::string> faces = {
            "../../../../assets/textures/test/skybox/right.jpg",
            "../../../../assets/textures/test/skybox/left.jpg",
            "../../../../assets/textures/test/skybox/top.jpg",
            "../../../../assets/textures/test/skybox/bottom.jpg",
            "../../../../assets/textures/test/skybox/front.jpg",
            "../../../../assets/textures/test/skybox/back.jpg"
    };
    Cubemap* skycubemap = new Cubemap(faces);
    SharedMesh skydome = SharedMesh(Mesh::sphere(10, true));
    GLint MVPsky;

    SkySystem* skySystem = nullptr;

public:
    Sky(SkySystem* skySystem = nullptr) : skySystem(skySystem)
    {
        VertexBuffer::uploadSingleMesh(skydome);

        MVPsky = skyShader.uniformLocation("MVP");
    }

    void render();
};


