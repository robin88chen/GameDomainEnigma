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
    class EffectMaterialCompiled : public Frameworks::IResponseEvent
    {
    public:
        EffectMaterialCompiled(const Frameworks::Ruid& request_ruid, const std::string& name, EffectMaterialPtr eff)
            : IResponseEvent(request_ruid), m_name(name), m_effect(std::move(eff)) {};
        const std::string& getName() { return m_name; }
        bool HasEffect() { return m_effect != nullptr; }
        EffectMaterialPtr GetEffect() { return m_effect; }

    private:
        std::string m_name;
        EffectMaterialPtr m_effect;
    };
    class CompileEffectMaterialFailed : public Frameworks::IResponseEvent
    {
    public:
        CompileEffectMaterialFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};
        const std::string& getName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }

    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // _EFFECT_RESPONSES_H
