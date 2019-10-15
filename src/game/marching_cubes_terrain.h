//
// Created by sneeuwpop on 14-10-19.
//

#pragma once


#include "../util/debug/gizmos.h"

// FOR MORE INFO SEE:  http://paulbourke.net/geometry/polygonise/
class MarchingCubesTerrain
{

private:

    static const int size = 6;
    float noisefield[size][size][size] = {0};

    Gizmos gizmos;



public:
    MarchingCubesTerrain();

    void render();

    int getCubeIndex(int x, int y, int z, float falloff);
};


