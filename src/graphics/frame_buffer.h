//
// Created by sneeuwpop on 05-01-20.
//

#pragma once


#include <vector>
#include <glad/glad.h>
#include "texture.h"

/**
 * 99% stolen from https://github.com/hilkojj/cpp-game-utils
 */
class FrameBuffer
{

public:

    const GLuint id, width, height, samples;

    Texture *colorTexture = nullptr;
    Texture *depthTexture = nullptr;


    FrameBuffer(GLuint width, GLuint height, GLuint samples=0);

    ~FrameBuffer();

    void bind();

    void unbind();

    /// \param textureFormat  GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_RG, GL_RGB, GL_RGBA
    /// \param magFilter GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    /// \param minFilter GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    void addColorTexture(GLuint textureFormat = GL_RGB, GLuint magFilter = GL_LINEAR, GLuint minFilter = GL_LINEAR);
    /// \param format GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, or GL_STENCIL_INDEX8
    void addColorBuffer(GLuint format = GL_RGBA4);


    /// \param magFilter GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    /// \param minFilter GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR
    void addDepthTexture(GLuint magFilter = GL_LINEAR, GLuint minFilter = GL_LINEAR);
    void addDepthBuffer();

    void renderGUI();

private:
    FrameBuffer *sampled = NULL;

    static void unbindCurrent();

};
