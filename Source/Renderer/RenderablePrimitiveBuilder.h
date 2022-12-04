/*********************************************************************
 * \file   RenderablePrimitiveBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_BUILDER_H
#define _RENDERABLE_PRIMITIVE_BUILDER_H

#include "RenderablePrimitivePolicies.h"
#include "Frameworks/SystemService.h"
#include "GameEngine/Primitive.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "Frameworks/EventSubscriber.h"
#include <queue>
#include <mutex>
#include <system_error>

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderablePrimitiveBuilder : Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        RenderablePrimitiveBuilder(Frameworks::ServiceManager* mngr);
        RenderablePrimitiveBuilder(const RenderablePrimitiveBuilder&) = delete;
        RenderablePrimitiveBuilder(RenderablePrimitiveBuilder&&) = delete;
        ~RenderablePrimitiveBuilder() override;
        RenderablePrimitiveBuilder& operator=(const RenderablePrimitiveBuilder&) = delete;
        RenderablePrimitiveBuilder& operator=(RenderablePrimitiveBuilder&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        error BuildPrimitive(const RenderablePrimitivePolicy& policy);

    protected:
        void BuildRenderablePrimitive(const RenderablePrimitivePolicy& policy);
        void BuildMeshPrimitive(const MeshPrimitivePolicy& policy);

        void OnGeometryDataBuilt(const Frameworks::IEventPtr& e);
        void OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e);
        void OnRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e);

    protected:
        std::queue<RenderablePrimitivePolicy> m_policies;
        std::mutex m_policiesLock;
        bool m_isCurrentBuilding;
        RenderablePrimitivePolicy m_currentPolicy;
        std::shared_ptr<Engine::Primitive> m_builtPrimitive;
        std::shared_ptr<Engine::GeometryData> m_builtGeometry;
        std::shared_ptr<Engine::RenderBuffer> m_builtRenderBuffer;

        Frameworks::EventSubscriberPtr m_onGeometryDataBuilt;
        Frameworks::EventSubscriberPtr m_onBuildGeometryDataFailed;
        Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
        Frameworks::EventSubscriberPtr m_onBuildRenderBufferFailed;
    };
}

#endif // _RENDERABLE_PRIMITIVE_BUILDER_H
