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
#include <string>
#include <vector>

namespace Enigma::Renderer
{
    class RenderablePrimitivePolicy
    {
    public:
        RenderablePrimitivePolicy() { m_ruid = Frameworks::Ruid::Generate(); }
        virtual ~RenderablePrimitivePolicy() = default;

        Frameworks::Ruid GetRuid() { return m_ruid; }

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }

    protected:
        Frameworks::Ruid m_ruid;
        std::string m_name;
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

    protected:
        Engine::GeometryDataPolicy m_geometryPolicy;
        std::vector<Engine::EffectMaterialPolicy> m_effectPolicies;
    };
}

#endif // _RENDERABLE_PRIMITIVE_POLICIES_H
