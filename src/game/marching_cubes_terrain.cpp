//
// Created by sneeuwpop on 14-10-19.
//

#include "marching_cubes_terrain.h"
#include "marching_cube.hpp"
#include "../util/math/math.h"
#include "vector"

#define LOOP3D(N, X,Y,Z, CODE) \
    for (int X=0; x<N; X++) { \
    for (int Y=0; y<N; Y++) { \
    for (int Z=0; z<N; Z++) { \
        CODE \
    }}} \


MarchingCubesTerrain::MarchingCubesTerrain()
{
    LOOP3D(size, x,y,z,
       noisefield[x][y][z] = (x==0 || x == size-1 || y==0 || y == size-1 || z==0 || z == size-1)? 1 : MATH::random();
    )
}

int MarchingCubesTerrain::getCubeIndex(int x, int y, int z, float falloff) {
    int cubeIndex = 0;

    const static int CUBE[8*3] {
        0, 0, 0,   1, 0, 0,   1, 0, 1,   0, 0, 1,
        0, 1, 0,   1, 1, 0,   1, 1, 1,   0, 1, 1,
    };

    for (unsigned int i=0; i < 8; i++) {
        if (noisefield[x + CUBE[i*3 +0]][y + CUBE[i*3 +1]][z + CUBE[i*3 +2]] < falloff)
            cubeIndex += ((u_int)1 << i);
    }
    return cubeIndex;
}

void MarchingCubesTerrain::render()
{
    LOOP3D(size, x,y,z, gizmos.drawCube(vec3(x,y,z), 0.1, vec4(noisefield[x][y][z])); )

    LOOP3D(size-1, x,y,z,
        int cubeIndex = getCubeIndex(x,y,z, 0.8f);
        for (int i = 0; i < 15; i += 3) {
            if (TRI_TABLE[cubeIndex][i] == -1) break;
            gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], COLOR::RED);
            gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], COLOR::RED);
            gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], COLOR::RED);
        }
    )

}