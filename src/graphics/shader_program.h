//
// Created by sneeuwpop on 20-6-19.
//

#pragma once

class ShaderProgram
{
private:
    GLuint programId; // TODO lookup what the standards are about private variables

public:
    static ShaderProgram fromAssetFiles(const char *vertPath, const char *fragPath);

    ShaderProgram(const char* vertexSource, const char* fragSource);
    ~ShaderProgram();

    void use();
    GLuint getId() { return programId; }
};