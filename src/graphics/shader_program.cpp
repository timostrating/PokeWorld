//
// Created by sneeuwpop on 20-6-19.
//

#include <iostream>
#include <glad/glad.h>
#include "shader_program.h"
#include "../util/io/file.h"

GLuint compile_shader(GLenum shaderType, const char *src)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char *buf = (char*)malloc(maxLength+1);
        glGetShaderInfoLog(shader, maxLength, &maxLength, buf);
        printf("%s\n", buf);
        free(buf);
        return 0;
    }

    return shader;
}

ShaderProgram::ShaderProgram(const char* vertexSource, const char* fragSource) {

    programId = glCreateProgram();

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragSource);

    glAttachShader(programId, vs);
    glAttachShader(programId, fs);

    glBindAttribLocation(programId, 0, "a_pos"); // TODO:

    glLinkProgram(programId);

}

ShaderProgram ShaderProgram::fromAssetFiles(const char *vertPath, const char *fragPath)
{
    std::string vertCode = File::readAssetAsString(vertPath);
    std::string fragCode = File::readAssetAsString(fragPath);
    return ShaderProgram(vertCode.c_str(), fragCode.c_str());
}

void ShaderProgram::use() {
    glUseProgram(programId);
}

ShaderProgram::~ShaderProgram() {} // TODO:
