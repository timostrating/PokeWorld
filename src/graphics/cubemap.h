//
// Created by sneeuwpop on 08-12-19.
//

#pragma once


#include <glad/glad.h>
#include <string>
#include <vector>
#include "shader_program.h"

class Cubemap {

private:
    GLuint textureId;
    unsigned char *imagePtr;

public:
//    static Cubemap fromAssetFile(const char *imgPath);

    Cubemap(std::vector<std::string> faces);
    ~Cubemap();

    void bind(unsigned int cubemapI);
    void bind(unsigned int cubemapI, const ShaderProgram &shader, const char *name);
};