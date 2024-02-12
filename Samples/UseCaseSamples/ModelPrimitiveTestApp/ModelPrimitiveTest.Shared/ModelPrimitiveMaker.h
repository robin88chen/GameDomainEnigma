/*********************************************************************
 * \file   ModelPrimitiveMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_MAKER_H
#define _MODEL_PRIMITIVE_MAKER_H

#include "Primitives/PrimitiveId.h"
#include "Geometries/GeometryId.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/MeshPrimitive.h"

class ModelPrimitiveMaker
{
public:
    static std::shared_ptr<Enigma::Renderables::ModelPrimitive> makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id);
    static std::shared_ptr<Enigma::Renderables::MeshPrimitive> makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id);
};

#endif // _MODEL_PRIMITIVE_MAKER_H
