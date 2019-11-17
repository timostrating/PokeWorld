//
// Created by sneeuwpop on 24-10-19.
//

#include <iostream>
#include "texture.h"
#include "shader_program.h"

Texture::Texture()
{
    glGenTextures(1, &textureId);
    std::cout << "Texture " << textureId << " created\n";

    bind(0);

    // Wraping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Black/white checkerboard
    float pixels[] = {
            0.2f, 0.2f, 0.2f,   0.8f, 0.8f, 0.8f,
            0.8f, 0.8f, 0.8f,   0.2f, 0.2f, 0.2f
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(const unsigned int textureI)
{
    glActiveTexture(GL_TEXTURE0 + textureI);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::bind(const unsigned int textureI, const ShaderProgram &shader, const char *name)
{
    bind(textureI);
    glUniform1i(shader.uniformLocation(name), textureI);
}

Texture Texture::fromAssetFile(const char *imgPath)
{
    return Texture(); // TODO
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureId);
    std::cout << "Texture " << textureId << " destroyed\n";
}


