//
// Created by sneeuwpop on 20-6-19.
//

#ifndef POKE_SHADER_PROGRAM_H
#define POKE_SHADER_PROGRAM_H

class ShaderProgram
{
public:
    static ShaderProgram fromAssetFiles(const char *vertPath, const char *fragPath);

    ShaderProgram(const char* vertexSource, const char* fragSource);
    ~ShaderProgram();

    void use();

private:
    GLuint programId;
};


#endif //POKE_SHADER_PROGRAM_H
