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

    // format can be GL_RGB or GL_RGBA. magFilter and minFilter can be GL_LINEAR for example.
    void addColorTexture(GLuint format, GLuint magFilter, GLuint minFilter);

    void addColorBuffer(GLuint format);

    void addDepthTexture(GLuint magFilter, GLuint minFilter);

    void addDepthBuffer();

    void renderGUI();

private:
    FrameBuffer *sampled = NULL;

    static void unbindCurrent();

};
