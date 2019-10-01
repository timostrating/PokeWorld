//
// Created by sneeuwpop on 12-09-19.
//

#include "lsystem.h"
#include "../../external/catch.hpp"

#define str std::string


void LSystem::addPattern(char key, std::string value) {
    patterns[key] = value;
}

std::string LSystem::getStr() {
    return str_value;
}

void LSystem::apply() {
    std::string newString;
    for (char c : str_value)
        newString.append( (patterns.find(c) != patterns.end()) ? str(patterns[c]) : str(1, c));
    str_value = newString;
}

void LSystem::applyNtimes(int n) {
    for (int i=0; i < n; i++)
        apply();
}


TEST_CASE("LSystem", "basic test") {
    LSystem l = LSystem("X");
    l.addPattern('X', "FX");
    l.apply();
    REQUIRE(l.getStr() == "FX");
}