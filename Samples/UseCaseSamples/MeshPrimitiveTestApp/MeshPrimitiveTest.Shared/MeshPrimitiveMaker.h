/*********************************************************************
 * \file   MeshPrimitiveMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_MAKER_H
#define _MESH_PRIMITIVE_MAKER_H

#include "Renderables/MeshPrimitive.h"
#include <memory>

class MeshPrimitiveMaker
{
public:
    static std::shared_ptr<Enigma::Renderables::MeshPrimitive> makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id);
};

#endif // _MESH_PRIMITIVE_MAKER_H
