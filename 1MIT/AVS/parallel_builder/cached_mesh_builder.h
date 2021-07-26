/**
 * @file    cached_mesh_builder.h
 *
 * @author  Jakub Sadilek (xsadil07) <xsadil07@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using pre-computed field
 *
 * @date    14.12.2020
 **/

#ifndef CACHED_MESH_BUILDER_H
#define CACHED_MESH_BUILDER_H

#include <vector>
#include "base_mesh_builder.h"

class CachedMeshBuilder : public BaseMeshBuilder
{
public:
    CachedMeshBuilder(unsigned gridEdgeSize);

protected:
    float *cacheArr;
    std::vector<Triangle_t> mTriangles; ///< Temporary array of triangles

    unsigned marchCubes(const ParametricScalarField &field);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    float originalEval(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }
};

#endif // CACHED_MESH_BUILDER_H
