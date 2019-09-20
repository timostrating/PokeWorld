//
// Created by sneeuwpop on 12-09-19.
//

#include <sstream>
#include <assert.h>
#include "Lsystem.h"

#define str std::string


void Lsystem::addPattern(char key, std::string value) {
    patterns[key] = value;
}

std::string Lsystem::getStr() {
    return str_value;
}

void Lsystem::apply() {
    std::string newString;
    for (char c : str_value)
        newString.append( (patterns.find(c) != patterns.end()) ? str(patterns[c]) : str(1, c));
    str_value = newString;
}

void Lsystem::applyNtimes(int n) {
    for (int i=0; i < n; i++)
        apply();
}

int test()
{
    Lsystem lSystem = Lsystem("X");
    lSystem.addPattern('X', "F[-X][+X],FX");
    assert(lSystem.getStr() == "X");
    lSystem.apply();
    assert(lSystem.getStr() == "F[-X][+X],FX");
    lSystem.apply();
    assert(lSystem.getStr() == "F[-F[-X][+X],FX][+F[-X][+X],FX],FF[-X][+X],FX");

    return 0;
}