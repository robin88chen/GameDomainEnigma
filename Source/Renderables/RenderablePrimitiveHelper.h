/*********************************************************************
 * \file   RenderablePrimitiveHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef RENDERABLE_PRIMITIVE_HELPER_H
#define RENDERABLE_PRIMITIVE_HELPER_H

#include "MeshPrimitive.h"
#include "RenderablePrimitiveDtoHelper.h"
#include "Primitives/PrimitivePersistenceLevel.h"

namespace Enigma::Renderables
{
    class MeshPrimitiveHelper
    {
    public:
        MeshPrimitiveHelper(const Primitives::PrimitiveId& id);

        MeshPrimitiveHelper& geometryId(const Geometries::GeometryId& id);
        MeshPrimitiveHelper& effect(const Engine::EffectMaterialId& id);
        MeshPrimitiveHelper& textureMap(const Engine::EffectTextureMapDtoHelper& texture_map);
        MeshPrimitiveHelper& renderListID(Renderer::Renderer::RenderListID id);
        MeshPrimitiveHelper& visualTechnique(const std::string& technique);
        MeshPrimitiveHelper& asNative(const std::string& file_at_path);

        std::shared_ptr<MeshPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

    protected:
        Primitives::PrimitiveId m_id;
        MeshPrimitiveDtoHelper m_dto;
    };
}

#endif // RENDERABLE_PRIMITIVE_HELPER_H
