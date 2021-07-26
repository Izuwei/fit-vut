/**
 * @file    tree_mesh_builder.h
 *
 * @author  Jakub Sadilek (xsadil07) <xsadil07@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    14.12.2020
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include "base_mesh_builder.h"

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    std::vector<Triangle_t> mTriangles; ///< Temporary array of triangles

    unsigned marchCubes(const ParametricScalarField &field);
    unsigned recursiveMarchCubes(const ParametricScalarField &field, Vec3_t<float> &offset, size_t size);
    int isEmpty(const ParametricScalarField &field, Vec3_t<float> &offset, size_t size);
    Vec3_t<float> calculateMidPoint(Vec3_t<float> &offset, size_t size);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }
};

#endif // TREE_MESH_BUILDER_H
