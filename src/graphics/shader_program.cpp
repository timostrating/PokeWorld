//
// Created by sneeuwpop on 20-6-19.
//

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "shader_program.h"
#include "../util/io/file.h"
#include "../util/nice_error.h"


void validateShader(GLint shaderId)
{
    GLint isCompiled = 0;
    GLint logLength = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (!isCompiled || logLength > 0)
    {
        std::vector<char> logText(logLength+1);
        glGetShaderInfoLog(shaderId, logLength, NULL, &logText[0]);
        printf("validateShader() ERROR: %s\n", &logText[0]);
    }
}

void validateProgram(GLint programId)
{
    GLint isCompiled = 0;
    GLint logLength = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &isCompiled);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (!isCompiled || logLength > 0)
    {
        std::vector<char> logText(logLength+1);
        glGetProgramInfoLog(programId, logLength, NULL, &logText[0]);
        printf("validateProgram() ERROR: %s\n", &logText[0]);
    }
}

GLuint compileShader(GLenum shaderType, const char *src)
{
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &src, NULL);
    glCompileShader(shaderId);

    validateShader(shaderId);

    return shaderId;
}



ShaderProgram::ShaderProgram(const char *vertexSource, const char *fragSource)
{
    programId = glCreateProgram();

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSource);

    glAttachShader(programId, vs);
    glAttachShader(programId, fs);

//    glBindAttribLocation(programId, 0, "a_pos"); // TODO:
    glLinkProgram(programId);

    validateProgram(programId);

    glDetachShader(programId, vs);
    glDetachShader(programId, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

ShaderProgram ShaderProgram::fromAssetFiles(const char *vertPath, const char *fragPath)
{
    std::string vertCode = File::readAssetAsString(vertPath);
    std::string fragCode = File::readAssetAsString(fragPath);
    return ShaderProgram(vertCode.c_str(), fragCode.c_str());
}

void ShaderProgram::begin()
{
    if (anyShaderActive)
        throw nice_err("you should have called .end() on your last Shader before you begin a new shader. \n");

    anyShaderActive = true;
    glUseProgram(programId);
}

void ShaderProgram::end()
{
    anyShaderActive = false;
    glUseProgram(0);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programId);
}
