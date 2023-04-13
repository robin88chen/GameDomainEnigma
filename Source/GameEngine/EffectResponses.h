/*********************************************************************
 * \file   EffectRequests.h
 * \brief  effect responses
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _EFFECT_RESPONSES_H
#define _EFFECT_RESPONSES_H

#include "Frameworks/Response.h"
#include "EffectMaterial.h"
#include <string>
#include <system_error>

namespace Enigma::Engine
{
    class CompileEffectMaterialResponse : public Frameworks::IResponse
    {
    public:
        CompileEffectMaterialResponse(const Frameworks::Ruid& request_ruid, const std::string& name, EffectMaterialPtr eff, std::error_code er) :
            IResponse(request_ruid), m_name(name), m_effect(std::move(eff)), m_error(er) {};
        const std::string& GetName() { return m_name; }
        bool HasEffect() { return m_effect != nullptr; }
        EffectMaterialPtr GetEffect() { return m_effect; }
        std::error_code GetErrorCode() const { return m_error; }

    private:
        std::string m_name;
        EffectMaterialPtr m_effect;
        std::error_code m_error;
    };
}

#endif // _EFFECT_RESPONSES_H
