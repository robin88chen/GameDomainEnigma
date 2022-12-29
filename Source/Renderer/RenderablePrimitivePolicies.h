/*********************************************************************
 * \file   RenderablePrimitivePolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_POLICIES_H
#define _RENDERABLE_PRIMITIVE_POLICIES_H

#include "Frameworks/ruid.h"
#include "GameEngine/GeometryDataPolicy.h"
#include "GameEngine/EffectMaterialPolicy.h"
#include "GameEngine/EffectTextureMapPolicy.h"
#include "RenderablePrimitiveDtos.h"
#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/EffectCompilingProfileDeserializer.h"
#include <string>
#include <vector>

namespace Enigma::Renderer
{
    class RenderablePrimitivePolicy
    {
    public:
        RenderablePrimitivePolicy() { m_ruid = Frameworks::Ruid::Generate(); }
        RenderablePrimitivePolicy(const RenderablePrimitivePolicy&) = default;
        RenderablePrimitivePolicy(RenderablePrimitivePolicy&&) = default;
        virtual ~RenderablePrimitivePolicy() = default;
        RenderablePrimitivePolicy& operator=(const RenderablePrimitivePolicy&) = default;
        RenderablePrimitivePolicy& operator=(RenderablePrimitivePolicy&&) = default;

        Frameworks::Ruid GetRuid() { return m_ruid; }

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const std::shared_ptr<Engine::IDtoDeserializer>& TheDtoDeserializer() const { return m_dtoDeserializer; }
        std::shared_ptr<Engine::IDtoDeserializer>& TheDtoDeserializer() { return m_dtoDeserializer; }
        [[nodiscard]] const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& TheEffectDeserializer() const { return m_effectDeserializer; }
        std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& TheEffectDeserializer() { return m_effectDeserializer; }

    protected:
        Frameworks::Ruid m_ruid;
        std::string m_name;
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
        std::shared_ptr<Engine::IEffectCompilingProfileDeserializer> m_effectDeserializer;
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

        [[nodiscard]] const Engine::GeometryDataPolicy& GeometryPolicy() const { return m_geometryPolicy; }
        Engine::GeometryDataPolicy& GeometryPolicy() { return m_geometryPolicy; }

        [[nodiscard]] const std::vector<Engine::EffectMaterialPolicy>& EffectPolicies() const { return m_effectPolicies; }
        std::vector<Engine::EffectMaterialPolicy>& EffectPolicies() { return m_effectPolicies; }

        [[nodiscard]] const std::vector<Engine::EffectTextureMapPolicy>& TexturePolicies() const { return m_texturePolicies; }
        std::vector<Engine::EffectTextureMapPolicy>& TexturePolicies() { return m_texturePolicies; }

        [[nodiscard]] const Engine::EffectTextureMapPolicy::TextureTuplePolicy& GetTextureTuplePolicy(unsigned tex_idx, unsigned tuple_idx) const;

    protected:
        Engine::GeometryDataPolicy m_geometryPolicy;
        std::vector<Engine::EffectMaterialPolicy> m_effectPolicies;
        std::vector<Engine::EffectTextureMapPolicy> m_texturePolicies;
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

    protected:
        MeshNodeTreeDto m_nodeTreeDto;
    };
}

#endif // _RENDERABLE_PRIMITIVE_POLICIES_H
