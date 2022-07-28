/*********************************************************************
 * \file   DrawPrimitiveTestApp_Jni.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DRAW_PRIMITIVE_TESTAPP_JNI_H
#define DRAW_PRIMITIVE_TESTAPP_JNI_H

#include "Application/AppDelegateAndroid.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererManager.h"

class ShaderBuilder;
class BufferBuilder;

class DrawPrimitiveTestApp : public Enigma::Application::AppDelegate
{
public:
    DrawPrimitiveTestApp(const std::string app_name);
    ~DrawPrimitiveTestApp();

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnShaderProgramCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnIndexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Graphics::IShaderProgramPtr m_program;
    Enigma::Graphics::IVertexBufferPtr m_vtxBuffer;
    Enigma::Graphics::IIndexBufferPtr m_idxBuffer;
    Enigma::Graphics::IVertexDeclarationPtr m_vtxDecl;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferBuilt;

    ShaderBuilder* m_shaderBuilder;
    BufferBuilder* m_bufferBuilder;
};

#endif // DRAW_PRIMITIVE_TESTAPP_JNI_H

