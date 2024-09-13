/*********************************************************************
 * \file   PrimitiveMeshMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PRIMITIVE_MESH_MAKER_H
#define _PRIMITIVE_MESH_MAKER_H

#include "GameEngine/BoundingVolume.h"
#include "Geometries/GeometryId.h"
#include "Primitives/PrimitiveId.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/TextureId.h"
#include <string>

class PrimitiveMeshMaker
{
public:
    static Enigma::Engine::GenericDto makeFloorGeometry(const Enigma::Geometries::GeometryId& id);
    static Enigma::Engine::GenericDto makeDoorGeometry(const Enigma::Geometries::GeometryId& id);
    static Enigma::Engine::GenericDto makeBoardGeometry(const Enigma::Geometries::GeometryId& id);
    static Enigma::Engine::GenericDto makeCubeGeometry(const Enigma::Geometries::GeometryId& id);
    static Enigma::Engine::GenericDto makeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id,
        const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id,
        const Enigma::Engine::TextureId& texture_id, const std::string& tex_semantic);

    static const Enigma::Engine::BoundingVolume& getFloorBound() { return m_floorBounding; }
    static const Enigma::Engine::BoundingVolume& getDoorBound() { return m_doorBounding; }
    static const Enigma::Engine::BoundingVolume& getBoardBound() { return m_boardBounding; }

private:
    static Enigma::Engine::BoundingVolume m_floorBounding;
    static Enigma::Engine::BoundingVolume m_doorBounding;
    static Enigma::Engine::BoundingVolume m_boardBounding;
};

#endif // _PRIMITIVE_MESH_MAKER_H
