/*********************************************************************
 * \file   RenderablePrimitivePolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_POLICIES_H
#define _RENDERABLE_PRIMITIVE_POLICIES_H

#include <memory>
#include "Geometries/GeometryDataPolicy.h"
#include "RenderablePrimitiveDtos.h"
#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/GenericPolicy.h"
#include "Animators/AnimatorDtos.h"
#include "GameEngine/EffectMaterialId.h"
#include <string>
#include <vector>
#include <optional>

namespace Enigma::Renderables
{
    class RenderablePrimitivePolicy : public Engine::GenericPolicy
    {
    public:
        RenderablePrimitivePolicy() = default;
        RenderablePrimitivePolicy(const RenderablePrimitivePolicy&) = default;
        RenderablePrimitivePolicy(RenderablePrimitivePolicy&&) = default;
        virtual ~RenderablePrimitivePolicy() override = default;
        RenderablePrimitivePolicy& operator=(const RenderablePrimitivePolicy&) = default;
        RenderablePrimitivePolicy& operator=(RenderablePrimitivePolicy&&) = default;

        [[nodiscard]] const Primitives::PrimitiveId& id() const { return m_id; }
        Primitives::PrimitiveId& id() { return m_id; }
        [[nodiscard]] const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        [[nodiscard]] const std::shared_ptr<Engine::IDtoDeserializer>& TheDtoDeserializer() const { return m_dtoDeserializer; }
        std::shared_ptr<Engine::IDtoDeserializer>& TheDtoDeserializer() { return m_dtoDeserializer; }

    protected:
        Primitives::PrimitiveId m_id;
        std::string m_name;
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
    };

    class MeshPrimitivePolicy : public RenderablePrimitivePolicy
    {
    public:
        MeshPrimitivePolicy();
        MeshPrimitivePolicy(const MeshPrimitivePolicy&) = default;
        MeshPrimitivePolicy(MeshPrimitivePolicy&&) = default;
        ~MeshPrimitivePolicy() override;
        MeshPrimitivePolicy& operator=(const MeshPrimitivePolicy&) = default;
        MeshPrimitivePolicy& operator=(MeshPrimitivePolicy&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& geometryFactoryDesc() const { return m_geometryFactoryDesc; }
        Engine::FactoryDesc& geometryFactoryDesc() { return m_geometryFactoryDesc; }

        [[nodiscard]] const Geometries::GeometryDataPolicy& geometryPolicy() const { return m_geometryPolicy; }
        Geometries::GeometryDataPolicy& geometryPolicy() { return m_geometryPolicy; }

        [[nodiscard]] const std::vector<Engine::EffectMaterialId>& effects() const { return m_effects; }
        std::vector<Engine::EffectMaterialId>& effects() { return m_effects; }

        [[nodiscard]] const std::vector<Engine::EffectTextureMapDto>& textureDtos() const { return m_textureDtos; }
        std::vector<Engine::EffectTextureMapDto>& textureDtos() { return m_textureDtos; }
        [[nodiscard]] const Renderer::Renderer::RenderListID& renderListId() const { return m_renderListId; }
        Renderer::Renderer::RenderListID& renderListId() { return m_renderListId; }
        [[nodiscard]] const std::string& visualTechniqueSelection() const { return m_visualTechniqueSelection; }
        std::string& visualTechniqueSelection() { return m_visualTechniqueSelection; }

        virtual std::shared_ptr<MeshPrimitive> createPrimitive() const;

    protected:
        Engine::FactoryDesc m_geometryFactoryDesc;
        Geometries::GeometryDataPolicy m_geometryPolicy;
        std::vector<Engine::EffectMaterialId> m_effects;
        std::vector<Engine::EffectTextureMapDto> m_textureDtos;
        Renderer::Renderer::RenderListID m_renderListId;
        std::string m_visualTechniqueSelection;
    };

    class SkinMeshPrimitivePolicy : public MeshPrimitivePolicy
    {
    public:
        SkinMeshPrimitivePolicy() : MeshPrimitivePolicy() {};
        SkinMeshPrimitivePolicy(const MeshPrimitivePolicy& policy) : MeshPrimitivePolicy(policy) {};
        SkinMeshPrimitivePolicy(const SkinMeshPrimitivePolicy&) = default;
        SkinMeshPrimitivePolicy(SkinMeshPrimitivePolicy&&) = default;
        ~SkinMeshPrimitivePolicy() = default;
        SkinMeshPrimitivePolicy& operator=(const SkinMeshPrimitivePolicy&) = default;
        SkinMeshPrimitivePolicy& operator=(SkinMeshPrimitivePolicy&&) = default;

        virtual std::shared_ptr<MeshPrimitive> createPrimitive() const override;
    };

    class ModelPrimitivePolicy : public RenderablePrimitivePolicy
    {
    public:
        ModelPrimitivePolicy();
        ModelPrimitivePolicy(const ModelPrimitivePolicy&) = default;
        ModelPrimitivePolicy(ModelPrimitivePolicy&&) = default;
        ~ModelPrimitivePolicy() override;
        ModelPrimitivePolicy& operator=(const ModelPrimitivePolicy&) = default;
        ModelPrimitivePolicy& operator=(ModelPrimitivePolicy&&) = default;

        [[nodiscard]] const MeshNodeTreeDto& nodeTreeDto() const { return m_nodeTreeDto; }
        MeshNodeTreeDto& nodeTreeDto() { return m_nodeTreeDto; }

        [[nodiscard]] const std::optional<Animators::ModelAnimatorDto>& modelAnimator() const { return m_animatorDto; }
        std::optional<Animators::ModelAnimatorDto>& modelAnimator() { return m_animatorDto; }

    protected:
        MeshNodeTreeDto m_nodeTreeDto;
        std::optional<Animators::ModelAnimatorDto> m_animatorDto;
    };
}

#endif // _RENDERABLE_PRIMITIVE_POLICIES_H
