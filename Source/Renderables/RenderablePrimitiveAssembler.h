/*********************************************************************
 * \file   RenderablePrimitiveAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef RENDERABLE_PRIMITIVE_ASSEMBLER_H
#define RENDERABLE_PRIMITIVE_ASSEMBLER_H

#include "Primitives/PrimitiveId.h"
#include "RenderablePrimitiveDtos.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Animators/AnimatorId.h"
#include "Primitives/PrimitivePersistenceLevel.h"

namespace Enigma::Renderables
{
    class MeshPrimitive;
    class SkinMeshPrimitive;
    class ModelPrimitive;

    class MeshPrimitiveAssembler
    {
    public:
        MeshPrimitiveAssembler(const Primitives::PrimitiveId& id);

        const Primitives::PrimitiveId& id() const { return m_id; }

        MeshPrimitiveAssembler& geometryId(const Geometries::GeometryId& id);
        MeshPrimitiveAssembler& effect(const Engine::EffectMaterialId& id);
        MeshPrimitiveAssembler& textureMap(const Engine::EffectTextureMapAssembler& texture_map);
        MeshPrimitiveAssembler& renderListID(Renderer::Renderer::RenderListID id);
        MeshPrimitiveAssembler& visualTechnique(const std::string& technique);
        MeshPrimitiveAssembler& asNative(const std::string& file_at_path);

        std::shared_ptr<MeshPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

        MeshPrimitiveDto dto() const;
        Engine::GenericDto toGenericDto() const;

    protected:
        Primitives::PrimitiveId m_id;
        MeshPrimitiveDto m_dto;
    };

    class SkinMeshPrimitiveAssembler
    {
    public:
        SkinMeshPrimitiveAssembler(const Primitives::PrimitiveId& id);

        const Primitives::PrimitiveId& id() const { return m_id; }

        SkinMeshPrimitiveAssembler& geometryId(const Geometries::GeometryId& id);
        SkinMeshPrimitiveAssembler& effect(const Engine::EffectMaterialId& id);
        SkinMeshPrimitiveAssembler& textureMap(const Engine::EffectTextureMapAssembler& texture_map);
        SkinMeshPrimitiveAssembler& renderListID(Renderer::Renderer::RenderListID id);
        SkinMeshPrimitiveAssembler& visualTechnique(const std::string& technique);
        SkinMeshPrimitiveAssembler& asNative(const std::string& file_at_path);

        std::shared_ptr<SkinMeshPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        Primitives::PrimitiveId m_id;
        SkinMeshPrimitiveDto m_dto;
    };

    class MeshNodeAssembler
    {
    public:
        MeshNodeAssembler(const std::string& name);

        const std::string& name() const { return m_name; }

        MeshNodeAssembler& localT_PosTransform(MathLib::Matrix4& transform);
        MeshNodeAssembler& meshPrimitive(const Primitives::PrimitiveId& id);
        MeshNodeAssembler& parentNode(const std::string& name);

        Engine::GenericDto toGenericDto();

    protected:
        friend class MeshNodeTreeAssembler;
        MeshNodeDto m_dto;
        std::string m_name;
        std::string m_parentName;
    };

    class MeshNodeTreeAssembler
    {
    public:
        MeshNodeTreeAssembler();

        MeshNodeTreeAssembler& addNode(const MeshNodeAssembler& node);

        Engine::GenericDto toGenericDto();

    protected:
        std::optional<unsigned> findNodeIndex(const std::string& name);

    protected:
        MeshNodeTreeDto m_dto;
        std::vector<MeshNodeAssembler> m_nodes;
    };

    class ModelPrimitiveAssembler
    {
    public:
        ModelPrimitiveAssembler(const Primitives::PrimitiveId& id);

        const Primitives::PrimitiveId& id() const { return m_id; }

        ModelPrimitiveAssembler& asNative(const std::string& file_at_path);
        ModelPrimitiveAssembler& meshNodeTree(const MeshNodeTreeAssembler& tree);
        ModelPrimitiveAssembler& animator(const Animators::AnimatorId& id);

        Engine::GenericDto toGenericDto();

        std::shared_ptr<ModelPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

    protected:
        Primitives::PrimitiveId m_id;
        ModelPrimitiveDto m_dto;
    };
}

#endif // RENDERABLE_PRIMITIVE_ASSEMBLER_H
