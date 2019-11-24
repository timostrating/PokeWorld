//
// Created by sneeuwpop on 24-10-19.
//

#include <iostream>
#include "texture.h"
#include "shader_program.h"
#include "../util/external/stb_image.hpp"

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
//    float pixels[] = {
//            0.2f, 0.2f, 0.2f,   0.8f, 0.8f, 0.8f,
//            0.8f, 0.8f, 0.8f,   0.2f, 0.2f, 0.2f
//    };
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

    int width, height, n;
    unsigned char *image = stbi_load("../../../../assets/textures/test/img_cheryl.jpg", &width, &height, &n, 0);
    // ... process data if not NULL ...
    // ... x = width, y = height, n = # 8-bit components per pixel ...
    // ... replace '0' with '1'..'4' to force that many components per pixel
    // ... but 'n' will always be the number that it would have been if you said 0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    // TODO: stbi_image_free(image);

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


