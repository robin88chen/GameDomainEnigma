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
    ShaderBuilder(Enigma::Graphics::IGraphicAPI::AsyncType asyncType);
    ~ShaderBuilder();

    void BuildVertexShader(const std::string& shader_name, const std::string& vtx_format_code, const std::string& shader_code);
    void BuildPixelShader(const std::string& shader_name, const std::string& shader_code);

private:
    void OnVertexShaderCreated(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Graphics::IGraphicAPI::AsyncType m_async;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexShaderCreated;
    std::string m_vtxShaderName;
    std::string m_vtxFormatCode;
    std::string m_vtxShaderCode;
};

#endif // SHADER_BUILDER_H
