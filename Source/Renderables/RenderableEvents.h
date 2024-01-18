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
#include "Primitives/Primitive.h"
#include <system_error>

namespace Enigma::Renderables
{
    class RenderablePrimitiveBuilt : public Frameworks::IEvent
    {
    public:
        RenderablePrimitiveBuilt(const Primitives::PrimitiveId& id, const std::shared_ptr<Primitives::Primitive> prim) : m_id(id), m_prim(prim) {};

        const Primitives::PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Primitives::Primitive>& primitive() { return m_prim; }

    private:
        Primitives::PrimitiveId m_id;
        std::shared_ptr<Primitives::Primitive> m_prim;
    };
    class BuildRenderablePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        BuildRenderablePrimitiveFailed(const Primitives::PrimitiveId& id, std::error_code er) : m_id(id), m_error(er) {};

        const Primitives::PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }

    private:
        Primitives::PrimitiveId m_id;
        std::error_code m_error;
    };
}

#endif // RENDERABLE_EVENTS_H
