//
// Created by sneeuwpop on 14-10-19.
//

#include "marching_cubes_terrain.h"
#include "marching_cube.hpp"
#include "../util/math/math.h"
#include "../graphics/camera.h"
#include "../graphics/texture.h"

#define LOOP3D(N, X,Y,Z, CODE) \
    for (int X=0; x<N; X++) { \
    for (int Y=0; y<N; Y++) { \
    for (int Z=0; z<N; Z++) { \
        CODE \
    }}} \


MarchingCubesTerrain::MarchingCubesTerrain()
{
    mvpId = terrainShader.uniformLocation("MVP");

    LOOP3D(size, x,y,z,
        noisefield[x][y][z] = (x==0 || x == size-1 || y==0 || y == size-1 || z==0 || z == size-1)? 1 : MATH::random() - ((y-size+3)*0.09); // remove some, based on y coordinate
        if ((x < 4 && z < 4) || (x < 4 && z > size-5) ||(x > size-5 && z < 4) || (x > size-5 && z > size-5)) // round the corners
            noisefield[x][y][z] += 0.2;
        if ((x < 2 && z < 2) || (x < 2 && z > size-3) ||(x > size-3 && z < 2) || (x > size-3 && z > size-3)) // round the corners
            noisefield[x][y][z] += 0.5;
    )

    LOOP3D(size-1, x,y,z,
        int cubeIndex = getCubeIndex(x,y,z, falloff);
        for (int i = 0; i < 15; i += 3) {
            if (TRI_TABLE[cubeIndex][i] == -1) break;
            vec3 p1 = vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]];
            vec3 p2 = vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]];
            vec3 p3 = vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]];
            mesh->vertices.insert(mesh->vertices.end(), {p1.x, p1.y, p1.z,  p2.x, p2.y, p2.z,  p3.x, p3.y, p3.z});
            mesh->nrOfVerts += 3;
        }
    )

    VertexBuffer::uploadSingleMesh(mesh);
}

int MarchingCubesTerrain::getCubeIndex(int x, int y, int z, float falloff)
{
    int cubeIndex = 0;

    const static int CUBE[8*3] {
        0, 0, 0,   1, 0, 0,   1, 0, 1,   0, 0, 1,
        0, 1, 0,   1, 1, 0,   1, 1, 1,   0, 1, 1,
    };

    for (unsigned int i=0; i < 8; i++) {
        if (noisefield[x + CUBE[i*3 +0]][y + CUBE[i*3 +1]][z + CUBE[i*3 +2]] < falloff)
            cubeIndex += ((uint)1 << i);
    }
    return cubeIndex;
}

void MarchingCubesTerrain::debugRender()
{
    LOOP3D(size, x,y,z, gizmos.drawCube(vec3(x,y,z), 0.1, vec4(noisefield[x][y][z], noisefield[x][y][z], noisefield[x][y][z], 0.3f)); )

    LOOP3D(size-1, x,y,z,
        int cubeIndex = getCubeIndex(x,y,z, falloff);
        for (int i=0 ;; i += 3) {
            if (TRI_TABLE[cubeIndex][i] == -1) break;
            gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], COLOR::RED);
            gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], COLOR::RED);
            gizmos.drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], COLOR::RED);
        }
    )
}

void MarchingCubesTerrain::render()
{
//    debugRender();
    terrainShader.use();
//    test.bind(0, terrainShader, "tex0");

    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
    mesh->render();
}