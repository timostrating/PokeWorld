//
// Created by sneeuwpop on 05-01-20.
//

#pragma once


#include <glad/glad.h>

class FrameBuffer
{
    GLuint fbo;

public:
    static FrameBuffer testBuffer();

    FrameBuffer() { glGenFramebuffers(1, &fbo); }
    ~FrameBuffer() { glDeleteFramebuffers(1, &fbo); }

    void bind();
    static void bindDefault();
};
