/*********************************************************************
 * \file   EffectEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_EVENTS_H
#define EFFECT_EVENTS_H

#include "Frameworks/Event.h"
#include "EffectMaterial.h"
#include <memory>
#include <system_error>

namespace Enigma::Engine
{
    /** effect events */
    class EffectMaterialCompiled : public Frameworks::IEvent
    {
    public:
        //! try rich event
        EffectMaterialCompiled(const std::string& filename, EffectMaterialPtr eff) :
            m_filename(filename), m_effect(std::move(eff)) {};
        const std::string& GetFilename() { return m_filename; }
        bool HasEffect() { return m_effect != nullptr; }
        EffectMaterialPtr GetEffect() { return std::move(m_effect); }
    private:
        std::string m_filename;
        EffectMaterialPtr m_effect;
    };
    class EffectMaterialCompileFailed : public Frameworks::IEvent
    {
    public:
        EffectMaterialCompileFailed(const std::string& filename, std::error_code er) :
            m_filename(filename), m_error(er) {};
        const std::string& GetFilename() { return m_filename; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_filename;
        std::error_code m_error;
    };
}

#endif // EFFECT_EVENTS_H
