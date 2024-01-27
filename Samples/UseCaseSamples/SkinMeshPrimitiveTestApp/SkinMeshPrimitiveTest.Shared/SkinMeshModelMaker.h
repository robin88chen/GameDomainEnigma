/*********************************************************************
 * \file   SkinMeshModelMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SKIN_MESH_MODEL_MAKER_H
#define _SKIN_MESH_MODEL_MAKER_H

#include "Primitives/PrimitiveId.h"
#include "Geometries/GeometryId.h"
#include "Animators/AnimatorId.h"

class SkinMeshModelMaker
{
public:
    static void makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Animators::AnimatorId& animator_id, const std::vector<std::string>& mesh_node_names);
    static void makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id);
};

#endif // _SKIN_MESH_MODEL_MAKER_H
