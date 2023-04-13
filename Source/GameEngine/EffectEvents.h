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
    /*class EffectMaterialCompiled : public Frameworks::IEvent
    {
    public:
        //! try rich event
        EffectMaterialCompiled(const std::string& filename, EffectMaterialPtr eff) :
            m_filename(filename), m_effect(std::move(eff)) {};
        const std::string& GetFilename() { return m_filename; }
        bool HasEffect() { return m_effect != nullptr; }
        EffectMaterialPtr GetEffect() { return m_effect; }
    private:
        std::string m_filename;
        EffectMaterialPtr m_effect;
    };*/
    class CompileEffectMaterialFailed : public Frameworks::IEvent
    {
    public:
        CompileEffectMaterialFailed(const std::string& filename, std::error_code er) :
            m_filename(filename), m_error(er) {};
        const std::string& GetFilename() { return m_filename; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_filename;
        std::error_code m_error;
    };
    class CompilingProfileDeserialized : public Frameworks::IEvent
    {
    public:
        CompilingProfileDeserialized(const Frameworks::Ruid& ruid, const EffectCompilingProfile& profile) :
            m_ruid(ruid), m_profile(profile) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        const EffectCompilingProfile& GetProfile() { return m_profile; }
    private:
        Frameworks::Ruid m_ruid;
        EffectCompilingProfile m_profile;
    };
    class DeserializeCompilingProfileFailed : public Frameworks::IEvent
    {
    public:
        DeserializeCompilingProfileFailed(const Frameworks::Ruid& ruid, std::error_code er) :
            m_ruid(ruid), m_error(er) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        Frameworks::Ruid m_ruid;
        std::error_code m_error;
    };
}

#endif // EFFECT_EVENTS_H
