/*********************************************************************
 * \file   RenderablePrimitiveEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_EVENTS_H
#define _RENDERABLE_PRIMITIVE_EVENTS_H

#include "Frameworks/Event.h"
#include "GameEngine/Primitive.h"
#include <system_error>

namespace Enigma::Renderer
{
    class RenderablePrimitiveBuilt : public Frameworks::IEvent
    {
    public:
        RenderablePrimitiveBuilt(const Frameworks::Ruid& ruid, const std::string& name, const Engine::PrimitivePtr prim) :
            m_ruid(ruid), m_name(name), m_prim(prim) {};
        const Frameworks::Ruid& GetRuid() const { return m_ruid; }
        const std::string& GetName() { return m_name; }
        const Engine::PrimitivePtr& GetPrimitive() { return m_prim; }
    private:
        Frameworks::Ruid m_ruid;
        std::string m_name;
        Engine::PrimitivePtr m_prim;
    };
    class BuildRenderablePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        BuildRenderablePrimitiveFailed(const Frameworks::Ruid& ruid, const std::string& name, std::error_code er) :
            m_ruid(ruid), m_name(name), m_error(er) {};
        const Frameworks::Ruid& GetRuid() const { return m_ruid; }
        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        Frameworks::Ruid m_ruid;
        std::string m_name;
        std::error_code m_error;
    };

}

#endif // _RENDERABLE_PRIMITIVE_EVENTS_H
