//
// Created by sneeuwpop on 23-6-19.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#define S(X) std::to_string(X).substr(0, 4)


namespace MATH
{
    const float PI = 3.1415926535f;
}

namespace VEC3
{
    const glm::vec3
            ZERO = glm::vec3(0, 0, 0),
            ONE  = glm::vec3(1, 1, 1),
            X    = glm::vec3(1, 0, 0),
            Y    = glm::vec3(0, 1, 0),
            Z    = glm::vec3(0, 0, 1);

    inline std::string toString(glm::vec3 vec)
    {
        return "("+S(vec.x)+", "+S(vec.y)+", "+S(vec.z)+")";
    }
}

namespace COLOR
{
    const glm::vec4
            DEFAULT    = glm::vec4(1,0,1,1), // pink is the default
            BLACK      = glm::vec4(0,0,0,1),
            WHITE      = glm::vec4(1,1,1,1),
            RED        = glm::vec4(1,0,0,1),
            GREEN      = glm::vec4(0,1,0,1),
            BLUE       = glm::vec4(0,0,1,1),
            YELLOW     = glm::vec4(1,1,0,1),
            PINK       = glm::vec4(1,0,1,1),
            LIGHT_BLUE = glm::vec4(0,1,1,1);
}

namespace MAT4
{
    const glm::mat4 IDENTITY = glm::mat4(1.0);

    inline std::string toString(glm::mat4 mat)
    {
        const float *m = (const float*)glm::value_ptr(mat);
        return "["+S(m[0 ])+", "+S(m[1 ])+", "+S(m[2 ])+", "+S(m[3 ])+", \n"+
               " "+S(m[4 ])+", "+S(m[5 ])+", "+S(m[6 ])+", "+S(m[7 ])+", \n"+
               " "+S(m[8 ])+", "+S(m[9 ])+", "+S(m[10])+", "+S(m[11])+", \n"+
               " "+S(m[12])+", "+S(m[13])+", "+S(m[14])+", "+S(m[15])+"] \n";
    }
}