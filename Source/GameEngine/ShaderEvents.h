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
#include "GraphicKernel/IShaderProgram.h"

namespace Enigma::Engine
{
    /** shader program events */
    class ShaderProgramBuilt : public Frameworks::IEvent
    {
    public:
        //! try rich event
        ShaderProgramBuilt(const std::string& name, const Graphics::IShaderProgramPtr& program) :
            m_name(name), m_program(program) {};
        const std::string& GetShaderName() { return m_name; }
        Graphics::IShaderProgramPtr GetProgram() { return m_program; }
    private:
        std::string m_name;
        Graphics::IShaderProgramPtr m_program;
    };
    class BuildShaderProgramFailed : public Frameworks::IEvent
    {
    public:
        BuildShaderProgramFailed(const std::string& name, std::error_code er) :
            m_name(name), m_error(er) {};
        const std::string& GetShaderName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // SHADER_EVENTS_H
