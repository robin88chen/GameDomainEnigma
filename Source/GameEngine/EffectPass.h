/*********************************************************************
 * \file   EffectPass.h
 * \brief  Effect pass value object
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef EFFECT_PASS_H
#define EFFECT_PASS_H

#include "EffectVariable.h"
#include <memory>
#include <optional>
#include <vector>
#include <string>

#include "Frameworks/optional_ref.hpp"

namespace Enigma::Graphics
{
    class IShaderProgram;
    using IShaderProgramPtr = std::shared_ptr<IShaderProgram>;
    class IDeviceSamplerState;
    using IDeviceSamplerStatePtr = std::shared_ptr<IDeviceSamplerState>;
    class IDeviceRasterizerState;
    using IDeviceRasterizerStatePtr = std::shared_ptr<IDeviceRasterizerState>;
    class IDeviceDepthStencilState;
    using IDeviceDepthStencilStatePtr = std::shared_ptr<IDeviceDepthStencilState>;
    class IDeviceAlphaBlendState;
    using IDeviceAlphaBlendStatePtr = std::shared_ptr<IDeviceAlphaBlendState>;
}
namespace Enigma::Engine
{
    struct EffectPassStates
    {
        std::optional<std::vector<Graphics::IDeviceSamplerStatePtr>> m_samplers;
        std::optional<std::vector<std::string>> m_samplerNames;
        std::optional<Graphics::IDeviceRasterizerStatePtr> m_rasterizer;
        std::optional<Graphics::IDeviceDepthStencilStatePtr> m_depth;
        std::optional<Graphics::IDeviceAlphaBlendStatePtr> m_blend;
    };

    class EffectPass
    {
    public:
        EffectPass(const Graphics::IShaderProgramPtr& program, const EffectPassStates& states);
        EffectPass(const EffectPass& pass);
        EffectPass(EffectPass&& pass);
        ~EffectPass();

        EffectPass& operator=(const EffectPass& pass);
        EffectPass& operator=(EffectPass&& pass) noexcept;

        void CommitVariables();
        void Apply();

        stdext::optional_ref<EffectVariable> GetVariableByName(const std::string& name);
        stdext::optional_ref<EffectVariable> GetVariableBySemantic(const std::string& semantic);
        stdext::optional_ref<const EffectVariable> GetVariableByName(const std::string& name) const;
        stdext::optional_ref<const EffectVariable> GetVariableBySemantic(const std::string& semantic) const;

    private:
        void BuildVariables();

    private:
        std::vector<EffectVariable> m_variables;

        Graphics::IShaderProgramPtr m_shader;
        std::vector<Graphics::IDeviceSamplerStatePtr> m_samplers;
        std::vector<std::string> m_samplerNames;
        Graphics::IDeviceRasterizerStatePtr m_rasterizerState;
        Graphics::IDeviceDepthStencilStatePtr m_depthState;
        Graphics::IDeviceAlphaBlendStatePtr m_blendState;
    };
}

#endif // EFFECT_PASS_H
