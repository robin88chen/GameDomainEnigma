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
#include "GameEngine/GeometryDataPolicy.h"
#include "RenderablePrimitiveDtos.h"
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/DtoDeserializer.h"
#include "Animators/AnimatorDtos.h"
#include <string>
#include <vector>
#include <optional>

namespace Enigma::Renderer
{
    class RenderablePrimitivePolicy
    {
    public:
        RenderablePrimitivePolicy() = default;
        RenderablePrimitivePolicy(const RenderablePrimitivePolicy&) = default;
        RenderablePrimitivePolicy(RenderablePrimitivePolicy&&) = default;
        virtual ~RenderablePrimitivePolicy() = default;
        RenderablePrimitivePolicy& operator=(const RenderablePrimitivePolicy&) = default;
        RenderablePrimitivePolicy& operator=(RenderablePrimitivePolicy&&) = default;

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const std::shared_ptr<Engine::IDtoDeserializer>& TheDtoDeserializer() const { return m_dtoDeserializer; }
        std::shared_ptr<Engine::IDtoDeserializer>& TheDtoDeserializer() { return m_dtoDeserializer; }

    protected:
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

        [[nodiscard]] const Engine::FactoryDesc& GeometryFactoryDesc() const { return m_geometryFactoryDesc; }
        Engine::FactoryDesc& GeometryFactoryDesc() { return m_geometryFactoryDesc; }

        [[nodiscard]] const Engine::GeometryDataPolicy& GeometryPolicy() const { return m_geometryPolicy; }
        Engine::GeometryDataPolicy& GeometryPolicy() { return m_geometryPolicy; }

        [[nodiscard]] const std::vector<Engine::EffectMaterialDto>& EffectDtos() const { return m_effectDtos; }
        std::vector<Engine::EffectMaterialDto>& EffectDtos() { return m_effectDtos; }

        [[nodiscard]] const std::vector<Engine::EffectTextureMapDto>& TextureDtos() const { return m_textureDtos; }
        std::vector<Engine::EffectTextureMapDto>& TextureDtos() { return m_textureDtos; }

        //[[nodiscard]] const Engine::EffectTextureMapPolicy::TextureTuplePolicy& GetTextureTuplePolicy(unsigned tex_idx, unsigned tuple_idx) const;

    protected:
        Engine::FactoryDesc m_geometryFactoryDesc;
        Engine::GeometryDataPolicy m_geometryPolicy;
        std::vector<Engine::EffectMaterialDto> m_effectDtos;
        std::vector<Engine::EffectTextureMapDto> m_textureDtos;
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

        [[nodiscard]] const MeshNodeTreeDto& NodeTreeDto() const { return m_nodeTreeDto; }
        MeshNodeTreeDto& NodeTreeDto() { return m_nodeTreeDto; }

        [[nodiscard]] const std::optional<Animators::ModelAnimatorDto>& TheModelAnimator() const { return m_animatorDto; }
        std::optional<Animators::ModelAnimatorDto>& TheModelAnimator() { return m_animatorDto; }

    protected:
        MeshNodeTreeDto m_nodeTreeDto;
        std::optional<Animators::ModelAnimatorDto> m_animatorDto;
    };
}

#endif // _RENDERABLE_PRIMITIVE_POLICIES_H
