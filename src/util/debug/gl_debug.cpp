//
// Created by sneeuwpop on 20-6-19.
//

#include <glad/glad.h>
#include <iostream>
#include "gl_debug.h"


namespace GL_DEBUG
{

    void printContext()
    {
        std::cout << "[OpenGL] " << glGetString(GL_VERSION) << std::endl;
        std::cout << "[Vendor] " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "[Renderer] " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "[GLSL] " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        // std::cout << "[Extensions] " << glGetString(GL_EXTENSIONS) << std::endl;
    }

    // CREDITS TO: https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/
    void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
        if (severity != GL_DEBUG_SEVERITY_LOW && severity != GL_DEBUG_SEVERITY_MEDIUM && severity != GL_DEBUG_SEVERITY_HIGH)
            return;

        std::cout << "---------------------opengl-callback-start------------" << std::endl;
        std::cout << "message: "<< message << std::endl;
        std::cout << "type: ";
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: std::cout << "ERROR"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "DEPRECATED_BEHAVIOR"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "UNDEFINED_BEHAVIOR"; break;
            case GL_DEBUG_TYPE_PORTABILITY: std::cout << "PORTABILITY"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "PERFORMANCE"; break;
            case GL_DEBUG_TYPE_OTHER: std::cout << "OTHER"; break;
        }
        std::cout << std::endl;

        std::cout << "id: " << id << std::endl;
        std::cout << "severity: ";
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_LOW: std::cout << "LOW"; break;
            case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "MEDIUM"; break;
            case GL_DEBUG_SEVERITY_HIGH: std::cout << "HIGH"; break;
        }
        std::cout << std::endl;
        std::cout << "---------------------opengl-callback-end--------------" << std::endl;
    }

    void enableGLDebug()
    {
        if (glDebugMessageCallback)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(openglCallbackFunction, nullptr);
        }
        else {
            std::cout << "_!_ glDebugMessageCallback not available _!_" << std::endl;
        }
    }

    void debugVertexNormals(SharedMesh mesh, mat4 transform, Gizmos *gizmos)
    {
        int size = mesh->attributes.getVertSize();
        for (int i=0; i<mesh->nrOfVerts * size; i+=size)
        {
            vec4 from = transform * vec4(mesh->vertices[i], mesh->vertices[i+1], mesh->vertices[i+2], 1.0);
            vec4 to = from + vec4(mesh->vertices[i+3], mesh->vertices[i+4], mesh->vertices[i+5], 1.0);
            gizmos->drawLine(from, to, COLOR::WHITE);
        }
    }
}
