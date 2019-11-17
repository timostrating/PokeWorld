//
// Created by sneeuwpop on 24-10-19.
//

#pragma once


#include <glad/glad.h>
#include "shader_program.h"

class Texture
{

private:
    GLuint textureId;

public:
    static Texture fromAssetFile(const char *imgPath);
    Texture();
    ~Texture();

    void bind(unsigned int textureI);
    void bind(unsigned int textureI, const ShaderProgram &shader, const char *name);

};


