/*********************************************************************
 * \file   RenderablePrimitiveDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef RENDERABLE_PRIMITIVE_DTO_HELPER_H
#define RENDERABLE_PRIMITIVE_DTO_HELPER_H

#include "Primitives/PrimitiveId.h"
#include "RenderablePrimitiveDtos.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/EffectDtoHelper.h"

namespace Enigma::Renderables
{
    class MeshPrimitiveDtoHelper
    {
    public:
        MeshPrimitiveDtoHelper(const Primitives::PrimitiveId& id);

        MeshPrimitiveDtoHelper& geometryId(const Geometries::GeometryId& id);
        MeshPrimitiveDtoHelper& effect(const Engine::EffectMaterialId& id);
        MeshPrimitiveDtoHelper& textureMap(const Engine::EffectTextureMapDtoHelper& texture_map);
        MeshPrimitiveDtoHelper& renderListID(Renderer::Renderer::RenderListID id);
        MeshPrimitiveDtoHelper& visualTechnique(const std::string& technique);
        MeshPrimitiveDtoHelper& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto();

    protected:
        MeshPrimitiveDto m_dto;
    };
}

#endif // RENDERABLE_PRIMITIVE_DTO_HELPER_H
