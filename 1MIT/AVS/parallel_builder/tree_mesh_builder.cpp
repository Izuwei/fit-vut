/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  Jakub Sadilek (xsadil07) <xsadil07@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    14.12.2020
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

Vec3_t<float> TreeMeshBuilder::calculateMidPoint(Vec3_t<float> &offset, size_t size)
{
    Vec3_t<float> midPoint((offset.x + size/2) * mGridResolution, (offset.y + size/2) * mGridResolution, (offset.z + size/2) * mGridResolution);
    
    return midPoint;
}

int TreeMeshBuilder::isEmpty(const ParametricScalarField &field, Vec3_t<float> &offset, size_t size)
{
    Vec3_t<float> midPoint = calculateMidPoint(offset, size);

    // expr = l + sqrt(3)/2 * a
    float expr = field.getIsoLevel() + (sqrt(3) / 2) * (size * mGridResolution);
    float F = evaluateFieldAt(midPoint, field);

    return F > expr;
}

unsigned TreeMeshBuilder::recursiveMarchCubes(const ParametricScalarField &field, Vec3_t<float> &offset, size_t size)
{
    unsigned totalTriangles = 0;

    if (size <= 1)
    {
        return buildCube(offset, field);
    }
    else
    {
        if (isEmpty(field, offset, size))
        {
            return 0;
        }
        else
        {
            for (int x = 0; x < 2; ++x)
            {
                for (int y = 0; y < 2; ++y)
                {
                    for (int z = 0; z < 2; ++z)
                    {
                        #pragma omp task shared(totalTriangles)
                        {
                            Vec3_t<float> newOffset(offset.x + x * (size/2), offset.y + y * (size/2), offset.z + z * (size/2));
                            unsigned triangles = recursiveMarchCubes(field, newOffset, size/2);

                            #pragma omp atomic update
                            totalTriangles += triangles;
                        }
                    }   
                }
            }
        }
    }
    #pragma omp taskwait
    return totalTriangles;
}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    // Suggested approach to tackle this problem is to add new method to
    // this class. This method will call itself to process the children.
    // It is also strongly suggested to first implement Octree as sequential
    // code and only when that works add OpenMP tasks to achieve parallelism.
    unsigned totalTriangles = 0;
    Vec3_t<float> cubeOffset(0, 0, 0);

    #pragma omp parallel
    #pragma omp single nowait
    totalTriangles = recursiveMarchCubes(field, cubeOffset, mGridSize);

    return totalTriangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    // NOTE: This method is called from "buildCube(...)"!

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

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    // NOTE: This method is called from "buildCube(...)"!

    // Store generated triangle into vector (array) of generated triangles.
    // The pointer to data in this array is return by "getTrianglesArray(...)" call
    // after "marchCubes(...)" call ends.
    #pragma omp critical
    mTriangles.push_back(triangle);
}
