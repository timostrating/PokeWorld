//
// Created by sneeuwpop on 30-6-19.
//

#pragma once

#include <iostream>
#include <string>


// CREDITS TO: https://github.com/hilkojj/cpp-game-utils/blob/master/source/utils/gu_error.h
class _nice_err : public std::runtime_error
{
public:
    _nice_err(const std::string &msg, const char *caller, const char *file, const unsigned int line)
            : std::runtime_error(msg)
    {
        std::cerr << "\n___nice_err___\n" <<  msg << "\n";
        std::cerr << file << ":" << line <<  "\n\n";
        std::cerr << "at " << caller << "\n\n";
    }
};

#define nice_err(msg) _nice_err(msg, __PRETTY_FUNCTION__, __FILE__, __LINE__)
