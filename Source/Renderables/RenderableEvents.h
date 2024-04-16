/*****************************************************************
 * \file   RenderableEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef RENDERABLE_EVENTS_H
#define RENDERABLE_EVENTS_H

#include "Frameworks/Event.h"
#include "Primitives/PrimitiveId.h"
#include <system_error>

namespace Enigma::Renderables
{
    class RenderablePrimitiveHydrated : public Frameworks::IEvent
    {
    public:
        RenderablePrimitiveHydrated(const Primitives::PrimitiveId& id)
            : m_id(id) {};
        const Primitives::PrimitiveId& id() const { return m_id; }

    protected:
        Primitives::PrimitiveId m_id;
    };
    class RenderablePrimitiveHydrationFailed : public Frameworks::IEvent
    {
    public:
        RenderablePrimitiveHydrationFailed(const Primitives::PrimitiveId& id, std::error_code er)
            : m_id(id), m_error(er) {};

        const Primitives::PrimitiveId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        Primitives::PrimitiveId m_id;
        std::error_code m_error;
    };
}

#endif // RENDERABLE_EVENTS_H
