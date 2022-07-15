/*********************************************************************
 * \file   DrawPrimitiveWithTexture.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DRAW_PRIMITIVE_WITH_TEXTURE_H
#define DRAW_PRIMITIVE_WITH_TEXTURE_H

#include "GraphicKernel/ITexture.h"
#include "GraphicKernel/IDeviceSamplerState.h"
#include "Application/AppDelegateWin32.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/RenderTarget.h"
#include "Frameworks/Timer.h"
#include "GameEngine/RendererManager.h"

class ShaderBuilder;
class BufferBuilder;
class TextureSamplerBuilder;

class DrawPrimitiveWithTextureApp : public Enigma::Application::AppDelegate
{
public:
    DrawPrimitiveWithTextureApp(const std::string app_name);
    ~DrawPrimitiveWithTextureApp();

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnShaderProgramCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnIndexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e);
    void OnSamplerBuilt(const Enigma::Frameworks::IEventPtr& e);

    void BuildVariables();

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Graphics::IShaderProgramPtr m_program;
    Enigma::Graphics::IVertexBufferPtr m_vtxBuffer;
    Enigma::Graphics::IIndexBufferPtr m_idxBuffer;
    Enigma::Graphics::IVertexDeclarationPtr m_vtxDecl;
    Enigma::Graphics::ITexturePtr m_texture;
    Enigma::Graphics::IDeviceSamplerStatePtr m_sampler;
    Enigma::Frameworks::Timer* m_timer;
    float m_tick;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureLoaded;
    Enigma::Frameworks::EventSubscriberPtr m_onSamplerBuilt;

    ShaderBuilder* m_shaderBuilder;
    BufferBuilder* m_bufferBuilder;
    TextureSamplerBuilder* m_textureBuilder;
};

#endif // DRAW_PRIMITIVE_WITH_TEXTURE_H
