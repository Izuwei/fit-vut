/**
 * @file    cached_mesh_builder.cpp
 *
 * @author  Jakub Sadilek (xsadil07) <xsadil07@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using pre-computed field
 *
 * @date    14.12.2020
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "cached_mesh_builder.h"

CachedMeshBuilder::CachedMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Cached")
{

}

unsigned CachedMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    size_t totalCubesCount = (mGridSize+1)*(mGridSize+1)*(mGridSize+1);
    cacheArr = new float[totalCubesCount]();

    #pragma omp parallel for schedule(static)
    for(size_t i = 0; i < totalCubesCount; ++i)
    {
        Vec3_t<float> cubeOffset( i % (mGridSize+1) * mGridResolution,
                                 (i / (mGridSize+1)) % (mGridSize+1) * mGridResolution,
                                  i / ((mGridSize+1)*(mGridSize+1)) * mGridResolution);
                                  
        cacheArr[i] = originalEval(cubeOffset, field);
    }

    totalCubesCount = (mGridSize)*(mGridSize)*(mGridSize);
    unsigned totalTriangles = 0;

    #pragma omp parallel for reduction(+ : totalTriangles) schedule(static)
    for(size_t i = 0; i < totalCubesCount; ++i)
    {
        Vec3_t<float> cubeOffset( i % (mGridSize+1),
                                 (i / (mGridSize+1)) % (mGridSize+1),
                                  i / ((mGridSize+1)*(mGridSize+1)));
                                  
        totalTriangles += buildCube(cubeOffset, field);
    }

    delete cacheArr;
    return totalTriangles;
}

float CachedMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    int index  = (int)(pos.x / mGridResolution + 0.5);
    index += ((int)(pos.y / mGridResolution + 0.5)) * (mGridSize+1);
    index += ((int)(pos.z / mGridResolution + 0.5)) * ((mGridSize+1)*(mGridSize+1));

    return cacheArr[index];
}

float CachedMeshBuilder::originalEval(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    // 1. Store pointer to and number of 3D points in the field
    //    (to avoid "data()" and "size()" call in the loop).
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    // 2. Find minimum square distance from points "pos" to any point in the
    //    field.
    for(unsigned i = 0; i < count; ++i)
    {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        // Comparing squares instead of real distance to avoid unnecessary
        // "sqrt"s in the loop.
        value = std::min(value, distanceSquared);
    }

    // 3. Finally take square root of the minimal square distance to get the real distance
    return sqrt(value);
}

void CachedMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    // NOTE: This method is called from "buildCube(...)"!

    // Store generated triangle into vector (array) of generated triangles.
    // The pointer to data in this array is return by "getTrianglesArray(...)" call
    // after "marchCubes(...)" call ends.
    #pragma omp critical
    mTriangles.push_back(triangle);
}
