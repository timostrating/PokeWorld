//
// Created by sneeuwpop on 20-6-19.
//

#pragma once

#include <glad/glad.h>

class ShaderProgram
{
private:
    GLuint programId;

public:
    static ShaderProgram fromAssetFiles(const char *vertPath, const char *fragPath);

    ShaderProgram(const char* vertexSource, const char* fragSource);
    ~ShaderProgram();

    void use();
//    GLuint getId() { return programId; }

    GLint uniformLocation(const char* name) const;
};