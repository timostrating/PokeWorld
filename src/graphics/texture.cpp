//
// Created by sneeuwpop on 24-10-19.
//

#include <iostream>
#include "texture.h"
#include "shader_program.h"
#include "../util/external/stb_image.hpp"
#include "../util/debug/nice_error.h"

Texture::Texture(const char *imgPath)
{
    glGenTextures(1, &textureId);
    std::cout << "Texture " << textureId << " " << imgPath << " created\n";

    bind(0); // TODO: support multiple textures

    // Wraping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (imgPath == nullptr)
    {
        float pixels[] = { // Black/white checkerboard
                0.2f, 0.2f, 0.2f,   0.8f, 0.8f, 0.8f,
                0.8f, 0.8f, 0.8f,   0.2f, 0.2f, 0.2f
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

    } else  {

        int width, height, n;
        // n = # 8-bit components per pixel. Replace '0' with '1'..'4' to force that many components per pixel
        // but 'n' will always be the number that it would have been if you said 0
        imagePtr = stbi_load(imgPath, &width, &height, &n, 0);

        if (imagePtr == nullptr)
            nice_error("Image loading error: image URI or file format is probably wrong.");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagePtr);
    }

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
    return Texture((&std::string("../../../../assets/").append(imgPath))->c_str()); // TODO
}

Texture Texture::testCheckerboard()
{
    return Texture(nullptr);
}


Texture::~Texture()
{
    if (imagePtr != nullptr)
        stbi_image_free(imagePtr);

    glDeleteTextures(1, &textureId);
    std::cout << "Texture " << textureId << " destroyed\n";
}


