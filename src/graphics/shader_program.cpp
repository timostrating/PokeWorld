//
// Created by sneeuwpop on 20-6-19.
//

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include "shader_program.h"
#include "../util/io/file.h"
#include "../util/debug/nice_error.h"


void validateShader(GLint shaderId)
{
    GLint isCompiled = 0;
    GLint logLength = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (!isCompiled || logLength > 1) // A driver may append a null terminator to a log in case there is no log
    {
        std::vector<char> logText(logLength+1);
        glGetShaderInfoLog(shaderId, logLength, NULL, &logText[0]);
        std::cout << "validateShader() ERROR: isCompiled = "<< ((isCompiled)?"TRUE":"FALSE") <<"; log = " << &logText[0] << "\n";
    }
}

void validateProgram(GLint programId)
{
    GLint isCompiled = 0;
    GLint logLength = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &isCompiled);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
    if (!isCompiled || logLength > 1) // A driver may append a null terminator to a log in case there is no log
    {
        std::vector<char> logText(logLength+1);
        glGetProgramInfoLog(programId, logLength, NULL, &logText[0]);
        std::cout << "validateProgram() ERROR: isCompiled = "<< ((isCompiled)?"TRUE":"FALSE") <<"; log = '" << &logText[0] << "'\n";
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

void ShaderProgram::use()
{
    glUseProgram(programId);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programId);
}

GLint ShaderProgram::uniformLocation(const char *name) const
{
    return glGetUniformLocation(programId, name);
}
