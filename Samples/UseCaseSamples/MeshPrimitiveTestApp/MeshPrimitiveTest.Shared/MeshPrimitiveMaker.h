/*********************************************************************
 * \file   MeshPrimitiveMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_MAKER_H
#define _MESH_PRIMITIVE_MAKER_H

#include "Renderer/RenderablePrimitivePolicies.h"
#include "GameEngine/EffectMaterialPolicy.h"
#include "GameEngine/EffectTextureMapPolicy.h"
#include "Renderer/MeshPrimitive.h"
#include <memory>

class MeshPrimitiveMaker
{
public:
    static void makeCubeMeshPrimitive(const Enigma::Engine::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id);
};

#endif // _MESH_PRIMITIVE_MAKER_H
