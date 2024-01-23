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

class ModelPrimitiveMaker
{
public:
    static void makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id);
    static void makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id);
};

#endif // _MODEL_PRIMITIVE_MAKER_H
