//
// Created by sneeuwpop on 24-10-19.
//

#include <iostream>
#include <imgui.h>
#include "texture.h"
#include "shader_program.h"
#include "../util/debug/nice_error.h"
#include "../util/external/stb_image.hpp"


Texture Texture::fromAssetFile(const char *imgPath, GLint a, GLint b)
{
    std::cout << " -- Are You sure about this? We stoped using textures.";
    return Texture((&std::string("../../../../assets/").append(imgPath))->c_str(), a, b);
}

Texture Texture::testCheckerboard()
{
    return Texture(nullptr);
}

Texture::Texture(const char *imgPath, GLint textureWrapping, GLint textureInterpolation)
{
    glGenTextures(1, &textureId);
    bind(textureId); // TODO: support multiple textures

    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapping); // GL_REPEAT or GL_MIRRORED_REPEAT or GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapping);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureInterpolation); // GL_REPEAT or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureInterpolation); // GL_NEAREST_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR or GL_LINEAR_MIPMAP_LINEAR

    if (imgPath == nullptr)
    {
        width = 2; height = 2;
        float pixels[] = { // Black/white checkerboard
                0.2f, 0.2f, 0.2f,   0.8f, 0.8f, 0.8f,
                0.8f, 0.8f, 0.8f,   0.2f, 0.2f, 0.2f
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
        std::cout << "Texture " << textureId << " test checkerboard" << " created (" << width << "," << height << ") \n";

    } else  {

        int n;
        // n = # 8-bit components per pixel. Replace '0' with '1'..'4' to force that many components per pixel
        // but 'n' will always be the number that it would have been if you said 0
        unsigned char *imagePtr = stbi_load(imgPath, &width, &height, &n, 0);

        if (imagePtr == nullptr)
            nice_error("Image loading error: image URI or file format is probably wrong. URL: "+std::string(imgPath));

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagePtr);
        stbi_image_free(imagePtr);

        std::cout << "Texture " << textureId << " " << std::string(imgPath) << " created (" << width << "," << height << ") \n";
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

Texture::~Texture()
{
    glDeleteTextures(1, &textureId);
    std::cout << "Texture " << textureId << " destroyed\n";
}

void Texture::renderGUI()
{
    ImGui::Text("id = %d   size = %d x %d", textureId, width, height);
    ImGui::Image((void*)(intptr_t)textureId, ImVec2(200, 200));
}


