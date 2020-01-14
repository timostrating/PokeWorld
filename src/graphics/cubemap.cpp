//
// Created by sneeuwpop on 08-12-19.
//

#include <iostream>
#include <vector>
#include "cubemap.h"
#include "../util/external/stb_image.hpp"

//Cubemap Cubemap::fromAssetFile()
//{
//
//    return Cubemap(faces);
//}

Cubemap::Cubemap(std::vector<std::string> faces)
{
    glGenTextures(1, &textureId);
    std::cout << "Cubemap " << textureId << " created\n";
    bind(0); // TODO: test support multiple textures

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
        else
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;

        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::bind(unsigned int cubemapI)
{
    glActiveTexture(GL_TEXTURE0 + cubemapI);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
}

void Cubemap::bind(unsigned int cubemapI, const ShaderProgram &shader, const char *name)
{
    bind(cubemapI);
    glUniform1i(shader.uniformLocation(name), cubemapI);
}

Cubemap::~Cubemap()
{
    glDeleteTextures(1, &textureId);
    std::cout << "Cubemap " << textureId << " destroyed\n";
}