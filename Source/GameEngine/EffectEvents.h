/*********************************************************************
 * \file   EffectEvents.h
 * \brief  effect events
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _EFFECT_EVENTS_H
#define _EFFECT_EVENTS_H

#include "Frameworks/Event.h"
#include "EffectMaterial.h"
#include "Frameworks/ruid.h"
#include <string>
#include <system_error>

namespace Enigma::Engine
{
    class EffectMaterial;
    class EffectMaterialSourceCompiled : public Frameworks::IEvent
    {
    public:
        EffectMaterialSourceCompiled(const EffectMaterialId& id) : m_id(id) {}

        const EffectMaterialId& id() { return m_id; }

    private:
        EffectMaterialId m_id;
    };
    class CompileEffectMaterialSourceFailed : public Frameworks::IEvent
    {
    public:
        CompileEffectMaterialSourceFailed(const EffectMaterialId& id, std::error_code er) : m_id(id), m_error(er) {};
        const EffectMaterialId& id() { return m_id; }
        std::error_code error() const { return m_error; }

    private:
        EffectMaterialId m_id;
        std::error_code m_error;
    };
    class EffectMaterialHydrated : public Frameworks::IEvent
    {
    public:
        EffectMaterialHydrated(const EffectMaterialId& source_id, const EffectMaterialId& id) : m_sourceId(source_id), m_id(id) {}

        const EffectMaterialId& sourceId() { return m_sourceId; }
        const EffectMaterialId& id() { return m_id; }

    private:
        EffectMaterialId m_sourceId;
        EffectMaterialId m_id;
    };
    class HydrateEffectMaterialFailed : public Frameworks::IEvent
    {
    public:
        HydrateEffectMaterialFailed(const EffectMaterialId& source_id, const EffectMaterialId& id, std::error_code er) : m_sourceId(source_id), m_error(er) {};
        const EffectMaterialId& sourceId() { return m_sourceId; }
        const EffectMaterialId& id() { return m_id; }
        std::error_code error() const { return m_error; }

    private:
        EffectMaterialId m_sourceId;
        EffectMaterialId m_id;
        std::error_code m_error;
    };
}

#endif // _EFFECT_RESPONSES_H
