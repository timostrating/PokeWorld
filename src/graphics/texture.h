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
    static Texture fromAssetFile(const char *imgPath, GLint textureWrapping = GL_REPEAT, GLint textureInterpolation = GL_LINEAR);
    static Texture testCheckerboard();

    Texture(const char *imgPath, GLint textureWrapping = GL_REPEAT, GLint textureInterpolation = GL_LINEAR);

    Texture(GLuint textureId) : textureId(textureId) {}

    ~Texture();

    void bind(unsigned int textureI);
    void bind(unsigned int textureI, const ShaderProgram &shader, const char *name);

};


