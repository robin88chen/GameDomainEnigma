/*********************************************************************
 * \file   ShaderEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef SHADER_EVENTS_H
#define SHADER_EVENTS_H

#include <system_error>
#include <Frameworks/Event.h>

namespace Enigma::Engine
{
    class ShaderProgramBuilt : public Frameworks::IEvent
    {
    public:
        ShaderProgramBuilt(const std::string& name) :
            m_name(name) {};
        const std::string& GetShaderName() { return m_name; }
    private:
        std::string m_name;
    };
    class ShaderProgramBuildFailed : public Frameworks::IEvent
    {
    public:
        ShaderProgramBuildFailed(const std::string& name, std::error_code er) :
            m_name(name), m_error(er) {};
        const std::string& GetShaderName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // SHADER_EVENTS_H
