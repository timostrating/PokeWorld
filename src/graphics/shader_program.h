//
// Created by sneeuwpop on 20-6-19.
//

#pragma once

class ShaderProgram
{
public:
    static ShaderProgram fromAssetFiles(const char *vertPath, const char *fragPath);

    ShaderProgram(const char* vertexSource, const char* fragSource);
    ~ShaderProgram();

    void begin();
    void end(); // TODO: end is not really required
    GLuint getId() { return programId; }

private:
    GLuint programId; // TODO lookup what the standards are about private variables
    static inline bool anyShaderActive = false;

};