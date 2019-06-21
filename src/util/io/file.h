//
// Created by sneeuwpop on 21-6-19.
//

#ifndef POKE_FILE_H
#define POKE_FILE_H

#include <string>

class File
{
public:
    static std::string readAssetAsString(const char *path);

private:
    static std::string readString(std::string *path);
};


#endif //POKE_FILE_H
