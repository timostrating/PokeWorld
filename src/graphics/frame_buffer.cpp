//
// Created by sneeuwpop on 05-01-20.
//

#include <imgui.h>
#include "frame_buffer.h"
#include "../util/debug/nice_error.h"
#include "../util/game.h"
#include "camera.h"


void FrameBuffer::unbindCurrent()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Camera::main->width, Camera::main->height);
}

GLuint create()
{
    GLuint id;
    glGenFramebuffers(1, &id);
    return id;
}

FrameBuffer::FrameBuffer(GLuint width, GLuint height, GLuint samples_)
        : id(create()), width(width), height(height),
          samples(
#ifdef EMSCRIPTEN
                  0
#else
                  samples_
#endif
          )
{
    std::cout << "FrameBuffer " << id << " created with " << samples << " samples\n";

    if (samples) sampled = new FrameBuffer(width, height);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &id);
    std::cout << "FrameBuffer " << id << " destroyed\n";
    delete sampled;
    delete colorTexture;
    delete depthTexture;
}

void FrameBuffer::bind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, width, height);
}

void FrameBuffer::unbind()
{
    unbindCurrent();
    if (!sampled) return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sampled->id);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);

    GLbitfield mask = 0;
    if (colorTexture) mask |= GL_COLOR_BUFFER_BIT;
    if (depthTexture) mask |= GL_DEPTH_BUFFER_BIT;

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);

    unbindCurrent();
}

void FrameBuffer::addColorTexture(GLuint format, GLuint magFilter, GLuint minFilter)
{
    if (sampled)
    {
        addColorBuffer(format);
        sampled->addColorTexture(format, magFilter, minFilter);
        colorTexture = sampled->colorTexture;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    // glDrawBuffer(GL_COLOR_ATTACHMENT0);

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    colorTexture = new Texture(texId, width, height);

    unbindCurrent();
}

void FrameBuffer::addColorBuffer(GLuint format)
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLuint id;
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);

    if (!sampled)
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, id);

    unbindCurrent();
}

void FrameBuffer::addDepthTexture(GLuint magFilter, GLuint minFilter)
{
    if (sampled)
    {
        addDepthBuffer();
        sampled->addDepthTexture(magFilter, minFilter);
        depthTexture = sampled->depthTexture;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, id);

#ifdef EMSCRIPTEN
    magFilter = minFilter = GL_NEAREST;
#endif

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texId, 0);
    depthTexture = new Texture(texId, width, height);

    unbindCurrent();
}

void FrameBuffer::addDepthBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    GLuint id;
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);

    if (!sampled)
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
    else
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT32F, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);

    unbindCurrent();
}

void FrameBuffer::renderGUI()
{
    ImGui::Text("Framebuffer %d", id);
    ImGui::Text("");

    if (colorTexture != nullptr) { ImGui::Text("pointer = %p", colorTexture); colorTexture->renderGUI(); }
    if (depthTexture != nullptr) { ImGui::Text("pointer = %p", depthTexture); depthTexture->renderGUI(); }
}