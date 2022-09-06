#include "EffectCompilingPolicies.h"

using namespace Enigma::Engine;

stdext::optional_ref<EffectPassProfile> EffectCompilingPolicy::FindPassWithSamplerState(const std::string& name)
{
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_samplers.empty()) continue;
            for (auto& samp : pass.m_samplers)
            {
                if (samp.m_name == name) return pass;
            }
        }
    }
    return std::nullopt;
}

stdext::optional_ref<EffectPassProfile> EffectCompilingPolicy::FindPassWithBlendState(const std::string& name)
{
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_blend.m_name == name) return pass;
        }
    }
    return std::nullopt;
}

stdext::optional_ref<EffectPassProfile> EffectCompilingPolicy::FindPassWithDepthState(const std::string& name)
{
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_depth.m_name == name) return pass;
        }
    }
    return std::nullopt;
}

stdext::optional_ref<EffectPassProfile> EffectCompilingPolicy::FindPassWithRasterizerState(const std::string& name)
{
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_rasterizer.m_name == name) return pass;
        }
    }
    return std::nullopt;
}

std::optional<unsigned> EffectCompilingPolicy::FindSamplerIndexInPass(const std::string& name)
{
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_samplers.empty()) continue;
            for (unsigned int i = 0; i < pass.m_samplers.size(); i++)
            {
                if (pass.m_samplers[i].m_name == name) return i;
            }
        }
    }
    return std::nullopt;
}
