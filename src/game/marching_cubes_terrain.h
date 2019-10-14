//
// Created by sneeuwpop on 14-10-19.
//

#pragma once


#include "../util/debug/gizmos.h"

// FOR MORE INFO SEE:  http://paulbourke.net/geometry/polygonise/
class MarchingCubesTerrain
{

private:

    static const int size = 5;
    const float noisefield[size][size][size] = {
        {
            {0.1, 0.7, 0.3},
            {0.2, 0.9, 0.5},
            {0.3, 0.8, 0.3},
        }, {
            {0.2, 0.9, 0.5},
            {0.1, 0.7, 0.3},
            {0.3, 0.8, 0.3},
        }, {
            {0.3, 0.8, 0.3},
            {0.2, 0.9, 0.5},
            {0.1, 0.7, 0.3},
        }
    };

    Gizmos gizmos;



public:
    MarchingCubesTerrain();

    void render();

    int getCubeIndex(int x, int y, int z, float falloff);
};


