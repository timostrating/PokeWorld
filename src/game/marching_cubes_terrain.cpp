//
// Created by sneeuwpop on 14-10-19.
//

#include "marching_cubes_terrain.h"
#include "marching_cube.hpp"

MarchingCubesTerrain::MarchingCubesTerrain()
{

}

int MarchingCubesTerrain::getCubeIndex(int x, int y, int z, float falloff) {
    int cubeIndex = 0;

    const static vec3 cubePoints[8] {
        vec3(0, 1, 1), vec3(1, 1, 1),
        vec3(0, 0, 1), vec3(0, 1, 1),
        vec3(0, 1, 0), vec3(1, 1, 0),
        vec3(0, 0, 0), vec3(0, 1, 0),
    };

    for (int i=0; i < 8; i++) {
        if (noisefield[x+(int)cubePoints[i].x][y+(int)cubePoints[i].y][z+(int)cubePoints[i].z] < falloff)
            cubeIndex += (1 << i);
    }
    return cubeIndex;
}

void MarchingCubesTerrain::render()
{
    for (int x=0; x<size; x++)
        for (int y=0; y<size; y++)
            for (int z=0; z<size; z++)
                gizmos.drawCube(vec3(x,y,z), 0.1, vec4(noisefield[x][y][z]));


    for (int x=0; x<size-1; x++) {
        for (int y=0; y<size-1; y++) {
            for (int z=0; z<size-1; z++) {

                int cubeIndex = getCubeIndex(x,y,z, 0.4f);
                for (int i = 0; i < 15; i += 3) {
                    if (TRI_TABLE[cubeIndex][i] == -1) break;
                    gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], COLOR::RED);
                    gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], COLOR::RED);
                    gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], COLOR::RED);
                }

            }
        }
    }

}