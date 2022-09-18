#include "EffectCompilingPolicies.h"

using namespace Enigma::Engine;

std::vector<std::reference_wrapper<EffectPassProfile>> EffectCompilingPolicy::FindPassesWithSamplerState(const std::string& name)
{
    std::vector<std::reference_wrapper<EffectPassProfile>> passes;
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_samplers.empty()) continue;
            for (auto& samp : pass.m_samplers)
            {
                if (samp.m_name == name)
                {
                    passes.emplace_back(pass);
                    break;
                }
            }
        }
    }
    return passes;
}

std::vector<std::reference_wrapper<EffectPassProfile>> EffectCompilingPolicy::FindPassesWithBlendState(const std::string& name)
{
    std::vector<std::reference_wrapper<EffectPassProfile>> passes;
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_blend.m_name == name) passes.emplace_back(pass);
        }
    }
    return passes;
}

std::vector<std::reference_wrapper<EffectPassProfile>> EffectCompilingPolicy::FindPassesWithDepthState(const std::string& name)
{
    std::vector<std::reference_wrapper<EffectPassProfile>> passes;
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_depth.m_name == name) passes.emplace_back(pass);
        }
    }
    return passes;
}

std::vector<std::reference_wrapper<EffectPassProfile>> EffectCompilingPolicy::FindPassesWithRasterizerState(const std::string& name)
{
    std::vector<std::reference_wrapper<EffectPassProfile>> passes;
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_rasterizer.m_name == name) passes.emplace_back(pass);
        }
    }
    return passes;
}

std::vector<std::reference_wrapper<EffectPassProfile>> EffectCompilingPolicy::FindPassesWithProgram(const std::string& name)
{
    std::vector<std::reference_wrapper<EffectPassProfile>> passes;
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_program.m_programName == name) passes.emplace_back(pass);
        }
    }
    return passes;
}

std::optional<unsigned> EffectCompilingPolicy::FindSamplerIndexInPass(const std::string& pass_name, const std::string& sampler_name)
{
    for (auto& tech : m_techniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_samplers.empty()) continue;
            if (pass.m_name != pass_name) continue;
            for (unsigned int i = 0; i < pass.m_samplers.size(); i++)
            {
                if (pass.m_samplers[i].m_name == sampler_name) return i;
            }
        }
    }
    return std::nullopt;
}
