/*********************************************************************
 * \file   ShaderBuilder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H
#include <string>
#include <Frameworks/Event.h>
#include <Frameworks/EventSubscriber.h>
#include <GraphicKernel/IGraphicAPI.h>

class ShaderBuilder
{
public:
    struct ShaderProgramBuildParameter
    {
        std::string m_programName;
        std::string m_vtxShaderName;
        std::string m_vtxFormatCode;
        std::string m_vtxShaderCode;
        std::string m_vtxLayoutName;
        std::string m_pixShaderName;
        std::string m_pixShaderCode;
    };
public:
    ShaderBuilder();
    ShaderBuilder(const ShaderBuilder&) = delete;
    ShaderBuilder(ShaderBuilder&&) = delete;
    ~ShaderBuilder();

    ShaderBuilder& operator=(const ShaderBuilder&) = delete;
    ShaderBuilder& operator=(ShaderBuilder&&) = delete;

    void BuildShaderProgram(const ShaderProgramBuildParameter& para);

    void BuildVertexShader(const std::string& shader_name, const std::string& vtx_format_code, 
        const std::string& shader_code, const std::string& layout_name);
    void BuildPixelShader(const std::string& shader_name, const std::string& shader_code);

private:
    void OnVertexShaderCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexShaderCompiled(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexLayoutCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnPixelShaderCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnPixelShaderCompiled(const Enigma::Frameworks::IEventPtr& e);

    void OnShaderBuilt(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Frameworks::EventSubscriberPtr m_onVertexShaderCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexShaderCompiled;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexLayoutCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onPixelShaderCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onPixelShaderCompiled;

    Enigma::Frameworks::EventSubscriberPtr m_onVertexShaderBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onPixelShaderBuilt;

    std::string m_programName;
    std::string m_vtxShaderName;
    std::string m_vtxFormatCode;
    std::string m_vtxShaderCode;
    std::string m_vtxLayoutName;
    std::string m_pixShaderName;
    std::string m_pixShaderCode;

    bool m_hasVtxShaderBuilt;
    bool m_hasPixShaderBuilt;
};

class VertexShaderBuilt : public Enigma::Frameworks::IEvent
{
public:
    VertexShaderBuilt(const std::string& name) :
        m_name(name) {};
    const std::string& GetShaderName() { return m_name; }
private:
    std::string m_name;
};
class PixelShaderBuilt : public Enigma::Frameworks::IEvent
{
public:
    PixelShaderBuilt(const std::string& name) :
        m_name(name) {};
    const std::string& GetShaderName() { return m_name; }
private:
    std::string m_name;
};

#endif // SHADER_BUILDER_H
