//
// Created by sneeuwpop on 12-09-19.
//

#pragma once

#include <string>
#include <map>

class LSystem
{
private:
    std::string str_value;
    std::map<char, std::string> patterns;

public:
    explicit LSystem(const char* start) : str_value(start) {};
    explicit LSystem(std::string &start) : str_value(start) {};
    ~LSystem() = default;

    void addPattern(char key, std::string value);
    std::string getStr();

    void apply();
    void applyNtimes(int n);


};