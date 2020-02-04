//
// Created by sneeuwpop on 14-10-19.
//

#include <imgui.h>
#include <iterator>
#include <map>
#include "marching_cubes_terrain.h"
#include "marching_cube.hpp"
#include "../util/math/math.h"
#include "../graphics/camera.h"
#include "../graphics/texture.h"
#include "../util/debug/gl_debug.h"

#define LOOP3D(N, X,Y,Z, CODE) \
    for (int X=0; x<N; X++) { \
    for (int Y=0; y<N; Y++) { \
    for (int Z=0; z<N; Z++) { \
        CODE \
    }}} \


SharedMesh smoothAndIndexify(SharedMesh mesh)
{
    std::vector<float> tmp;
    mesh->vertices.swap(tmp);

    std::unordered_map<int, unsigned short> memory;

    unsigned short index = 0;
    for (int i=0; i<mesh->nrOfVerts * mesh->attributes.getVertSize(); i+=mesh->attributes.getVertSize())
    {
        float a=tmp[i + 0], b=tmp[i + 1], c=tmp[i + 2];
        int v = static_cast<int>(a * pow(2.0f*15.0f, 3.0) + b * pow(2.0f*15.0f, 2.0) + c * 2.0f*15.0f);
        bool inMemory = memory.find(v) != memory.end();

        if (inMemory) {
            mesh->vertices[mesh->attributes.getVertSize() * memory.find(v)->second +3] += tmp[i+3];
            mesh->vertices[mesh->attributes.getVertSize() * memory.find(v)->second +4] += tmp[i+4];
            mesh->vertices[mesh->attributes.getVertSize() * memory.find(v)->second +5] += tmp[i+5];
            mesh->indicies.insert(mesh->indicies.end(), {memory.find(v)->second});
        } else {
            mesh->vertices.insert(mesh->vertices.end(), {a,b,c, tmp[i+3], tmp[i+4], tmp[i+5]});
            mesh->indicies.insert(mesh->indicies.end(), {index});
            memory.insert(std::pair<int, unsigned short>(v, index));
            index++;
        }
    }

    mesh->nrOfVerts = mesh->vertices.size() / mesh->attributes.getVertSize();
    mesh->nrOfIndices = mesh->indicies.size();
    return mesh;
}

MarchingCubesTerrain::MarchingCubesTerrain()
{
    MVP = terrainShader.uniformLocation("MVP");
    u_gradient = terrainShader.uniformLocation("u_gradient");

    srand(0);

    LOOP3D(size, x,y,z,
        noisefield[x][y][z] = MATH::random() - ((y-size+3)*0.09);
        if ((x < 4 && z < 4) || (x < 4 && z > size-5) ||(x > size-5 && z < 4) || (x > size-5 && z > size-5)) // round the corners
            noisefield[x][y][z] += 0.2;
        if ((x < 2 && z < 2) || (x < 2 && z > size-3) ||(x > size-3 && z < 2) || (x > size-3 && z > size-3)) // round the corners
            noisefield[x][y][z] += 0.5;

        if (y == size-2) {
            noisefield[x][y][z] = 0;
            if (x < size/2 -1 && z > size/2)
                noisefield[x][y][z] = 1.0;
        }
        if (y == size-3) {
            noisefield[x][y][z] = 0.0;
        }

        noisefield[x][y][z] = (x==0 || x == size-1 || y==0 || y == size-1 || z==0 || z == size-1)? 1 : noisefield[x][y][z];
    )

    LOOP3D(size-1, x,y,z,
        int cubeIndex = getCubeIndex(x,y,z, falloff);
        for (int i = 0; i < 15; i += 3) {
            if (TRI_TABLE[cubeIndex][i] == -1) break;
            vec3 p1 = 5.0f * (vec3(-5, -9.5 + 0.2, -5) + vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]]);
            vec3 p2 = 5.0f * (vec3(-5, -9.5 + 0.2, -5) + vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]]);
            vec3 p3 = 5.0f * (vec3(-5, -9.5 + 0.2, -5) + vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]]);
            vec3 n = normalize(cross(p2-p1, p3-p1));  // POSITION        NORMAL        POSITION        NORMAL        POSITION        NORMAL
            mesh->vertices.insert(mesh->vertices.end(), {p1.x,p1.y,p1.z, n.x,n.y,n.z,  p2.x,p2.y,p2.z, n.x,n.y,n.z,  p3.x,p3.y,p3.z, n.x,n.y,n.z});
            mesh->nrOfVerts += 3;
        }
    )

    mesh = smoothAndIndexify(mesh);
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

void MarchingCubesTerrain::debugRender(Gizmos* gizmos)
{
    mat4 transform = scale(translate(mat4(1), 5.0f * vec3(-5, -9.5 + 0.2, -5)), vec3(5.0f));
    LOOP3D(size, x,y,z, gizmos->drawCube(vec3(x,y,z), 0.01, vec4(noisefield[x][y][z], noisefield[x][y][z], noisefield[x][y][z], 0.1f), transform); )

    LOOP3D(size-1, x,y,z,
        int cubeIndex = getCubeIndex(x,y,z, falloff);
        for (int i=0 ;; i += 3) {
            if (TRI_TABLE[cubeIndex][i] == -1) break;
            gizmos->drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], vec4(0,0,0, 0.2), transform);
            gizmos->drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 1]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], vec4(0,0,0, 0.2), transform);
            gizmos->drawLine(vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 2]], vec3(x, y, z) + EDGE_POINTS[TRI_TABLE[cubeIndex][i + 0]], vec4(0,0,0, 0.2), transform);
        }
    )
}

void MarchingCubesTerrain::render(float time)
{
    terrainShader.use();
//    test.bind(0, terrainShader, "tex0");
    glUniformMatrix4fv(MVP, 1, GL_FALSE, &(Camera::main->combined * transform)[0][0]);
    glUniformMatrix4fv(u_gradient, 1, GL_FALSE, &(gradient)[0][0]);
    glUniform1f(terrainShader.uniformLocation("u_time"), time);
    mesh->render();

//    GL_DEBUG::debugVertexNormals(mesh, transform, &gizmos);
}

void MarchingCubesTerrain::renderGui()
{
    // TODO: something like ClipDistance is still required to make the reflections work correctly
//    ImGui::Text("MarchingCubesTerrain:");
//    ImGui::Text("");
//    ImGui::ColorEdit4("color1", &(gradient)[0][0]);
//    ImGui::ColorEdit4("color2", &(gradient)[1][0]);
//    ImGui::ColorEdit4("color3", &(gradient)[2][0]);
//    ImGui::ColorEdit4("color4", &(gradient)[3][0]);
}
