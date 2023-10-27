/*********************************************************************
 * \file   RenderableEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef RENDERABLE_EVENTS_H
#define RENDERABLE_EVENTS_H

#include "Frameworks/Event.h"
#include "GameEngine/Primitive.h"
#include <system_error>

namespace Enigma::Renderer
{
    class RenderablePrimitiveBuilt : public Frameworks::IResponseEvent
    {
    public:
        RenderablePrimitiveBuilt(const Frameworks::Ruid& request_ruid, const std::string& name, const Engine::PrimitivePtr prim)
            : IResponseEvent(request_ruid), m_name(name), m_prim(prim) {};

        const std::string& getName() { return m_name; }
        const Engine::PrimitivePtr& getPrimitive() { return m_prim; }

    private:
        std::string m_name;
        Engine::PrimitivePtr m_prim;
    };
    class BuildRenderablePrimitiveFailed : public Frameworks::IResponseEvent
    {
    public:
        BuildRenderablePrimitiveFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& getName() { return m_name; }
        std::error_code errorCode() const { return m_error; }

    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // RENDERABLE_EVENTS_H
