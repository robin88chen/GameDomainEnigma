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
    class RenderablePrimitiveBuilt : public Frameworks::IEvent
    {
    public:
        RenderablePrimitiveBuilt(const Engine::PrimitiveId& id, const std::shared_ptr<Engine::Primitive> prim) : m_id(id), m_prim(prim) {};

        const Engine::PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Engine::Primitive>& primitive() { return m_prim; }

    private:
        Engine::PrimitiveId m_id;
        std::shared_ptr<Engine::Primitive> m_prim;
    };
    class BuildRenderablePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        BuildRenderablePrimitiveFailed(const Engine::PrimitiveId& id, std::error_code er) : m_id(id), m_error(er) {};

        const Engine::PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }

    private:
        Engine::PrimitiveId m_id;
        std::error_code m_error;
    };
}

#endif // RENDERABLE_EVENTS_H
