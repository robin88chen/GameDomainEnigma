/*********************************************************************
 * \file   RenderElementBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _RENDER_ELEMENT_BUILDER_H
#define _RENDER_ELEMENT_BUILDER_H

#include "RenderElementBuildingPolicies.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/RenderBufferRepository.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialManager.h"
#include <memory>

namespace Enigma::Renderer
{
    class RendererManager;
    class RenderElement;

    class RenderElementBuilder
    {
    public:
        class RenderElementBuilt : public Frameworks::IEvent
        {
        public:
            RenderElementBuilt(const RenderElementPolicy& policy, const std::shared_ptr<RenderElement>& element) :
                m_policy(policy), m_element(element) {};
            const RenderElementPolicy& GetPolicy() const { return m_policy; }
            const std::shared_ptr<RenderElement>& GetElement() { return m_element; }

        private:
            RenderElementPolicy m_policy;
            std::shared_ptr<RenderElement> m_element;
        };

    public:
        RenderElementBuilder(RendererManager* host, Engine::RenderBufferRepository* buffer_repository,
            Engine::EffectMaterialManager* material_manager);
        ~RenderElementBuilder();

        void BuildRenderElement(const RenderElementPolicy& policy);

    protected:
        void OnRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e);
        void OnEffectMaterialCompiled(const Frameworks::IEventPtr& e);
        void OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e);

        void TryCompletingBuild();

    protected:
        RendererManager* m_hostManager;
        Engine::RenderBufferRepository* m_renderBufferRepository;
        Engine::EffectMaterialManager* m_effectMaterialManager;

        Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
        Frameworks::EventSubscriberPtr m_onBuildRenderBufferFailed;
        Frameworks::EventSubscriberPtr m_onEffectMaterialCompiled;
        Frameworks::EventSubscriberPtr m_onCompileEffectMaterialFailed;

        Engine::RenderBufferPtr m_buffer;
        Engine::EffectMaterialPtr m_effect;

        RenderElementPolicy m_policy;
    };
}

#endif // _RENDER_ELEMENT_BUILDER_H
