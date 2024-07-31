/*********************************************************************
 * \file   ShaderBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include "ShaderBuildingPolicies.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GraphicKernel/IVertexShader.h"
#include "GraphicKernel/IPixelShader.h"
#include "GraphicKernel/IShaderProgram.h"
#include <string>

namespace Enigma::Engine
{
    class ShaderRepository;

    class ShaderBuilder
    {
    private:
        /** inner events */
        class VertexShaderBuilt : public Frameworks::IEvent
        {
        public:
            VertexShaderBuilt(const std::string& name) :
                m_name(name) {};
            const std::string& shaderName() { return m_name; }
        private:
            std::string m_name;
        };
        class PixelShaderBuilt : public Frameworks::IEvent
        {
        public:
            PixelShaderBuilt(const std::string& name) :
                m_name(name) {};
            const std::string& shaderName() { return m_name; }
        private:
            std::string m_name;
        };

    public:
        class ShaderProgramBuilt : public Frameworks::IEvent
        {
        public:
            ShaderProgramBuilt(const std::string& name) :
                m_name(name) {};
            const std::string& shaderName() { return m_name; }
        private:
            std::string m_name;
        };

    public:
        ShaderBuilder(ShaderRepository* host);
        ShaderBuilder(const ShaderBuilder&) = delete;
        ShaderBuilder(ShaderBuilder&&) = delete;
        ~ShaderBuilder();

        ShaderBuilder& operator=(const ShaderBuilder&) = delete;
        ShaderBuilder& operator=(ShaderBuilder&&) = delete;

        void buildShaderProgram(const ShaderProgramPolicy& policy);

        Graphics::IShaderProgramPtr getProgram() { return m_program; }

    private:
        void onVertexShaderCreated(const Frameworks::IEventPtr& e);
        void onVertexShaderCompiled(const Frameworks::IEventPtr& e);
        void onVertexLayoutCreated(const Frameworks::IEventPtr& e);
        void onPixelShaderCreated(const Frameworks::IEventPtr& e);
        void onPixelShaderCompiled(const Frameworks::IEventPtr& e);
        void onShaderCompileFailed(const Frameworks::IEventPtr& e);

        void onShaderBuilt(const Frameworks::IEventPtr& e);
        void onShaderProgramCreated(const Frameworks::IEventPtr& e);
        void onShaderProgramLinkFailed(const Frameworks::IEventPtr& e);

    private:
        ShaderRepository* m_hostRepository;

        Frameworks::EventSubscriberPtr m_onVertexShaderCreated;
        Frameworks::EventSubscriberPtr m_onVertexShaderCompiled;
        Frameworks::EventSubscriberPtr m_onVertexLayoutCreated;
        Frameworks::EventSubscriberPtr m_onPixelShaderCreated;
        Frameworks::EventSubscriberPtr m_onPixelShaderCompiled;

        Frameworks::EventSubscriberPtr m_onVertexShaderCompileFailed;
        Frameworks::EventSubscriberPtr m_onPixelShaderCompileFailed;

        Frameworks::EventSubscriberPtr m_onVertexShaderBuilt;
        Frameworks::EventSubscriberPtr m_onPixelShaderBuilt;
        Frameworks::EventSubscriberPtr m_onShaderProgramCreated;

        Frameworks::EventSubscriberPtr m_onShaderProgramLinkFailed;

        ShaderProgramPolicy m_policy;

        Graphics::IVertexShaderPtr m_vtxShader;
        Graphics::IPixelShaderPtr m_pixShader;
        Graphics::IVertexDeclarationPtr m_layout;
        Graphics::IShaderProgramPtr m_program;
    };
}

#endif // SHADER_BUILDER_H
