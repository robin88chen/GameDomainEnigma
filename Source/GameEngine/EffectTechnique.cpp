#include "EffectTechnique.h"
#include "EngineErrors.h"
#include <cassert>

using namespace Enigma::Engine;

EffectTechnique::EffectTechnique(const std::string& name)
{
    m_name = name;
    m_effectPasses.clear();
    m_currentApplyPass = m_effectPasses.end();
}

EffectTechnique::EffectTechnique(const EffectTechnique& source_tech)
{
    m_name = source_tech.m_name;
    m_effectPasses = source_tech.m_effectPasses;
    m_currentApplyPass = m_effectPasses.end();
}

EffectTechnique::EffectTechnique(EffectTechnique&& source_tech) noexcept
{
    m_name = std::move(source_tech.m_name);
    m_effectPasses = std::move(source_tech.m_effectPasses);
    m_currentApplyPass = m_effectPasses.end();
}

EffectTechnique::~EffectTechnique()
{
    m_effectPasses.clear();
    m_currentApplyPass = m_effectPasses.end();
}

EffectTechnique& EffectTechnique::operator=(EffectTechnique&& source_tech) noexcept
{
    m_name = source_tech.m_name;
    m_effectPasses = source_tech.m_effectPasses;
    m_currentApplyPass = m_effectPasses.end();

    return *this;
}

EffectTechnique& EffectTechnique::operator=(const EffectTechnique& source_tech)
{
    m_name = std::move(source_tech.m_name);
    m_effectPasses = std::move(source_tech.m_effectPasses);
    m_currentApplyPass = m_effectPasses.end();

    return *this;
}

void EffectTechnique::AddPass(const EffectPass& pass)
{
    m_effectPasses.emplace_back(pass);
}

EffectPass& EffectTechnique::GetPassByIndex(unsigned index)
{
    assert(index <= m_effectPasses.size());
    return m_effectPasses[index];
}

const EffectPass& EffectTechnique::GetPassByIndex(unsigned index) const
{
    assert(index <= m_effectPasses.size());
    return m_effectPasses[index];
}

error EffectTechnique::ApplyFirstPass()
{
    m_currentApplyPass = m_effectPasses.begin();
    if (m_currentApplyPass == m_effectPasses.end())
    {
        return ErrorCode::effectPassEnd;
    }
    (*m_currentApplyPass).Apply();
    return ErrorCode::ok;
}

error EffectTechnique::ApplyNextPass()
{
    if (m_currentApplyPass == m_effectPasses.end())
    {
        return ErrorCode::ok;
    }
    ++m_currentApplyPass;
    if (m_currentApplyPass == m_effectPasses.end())
    {
        return ErrorCode::effectPassEnd;
    }
    (*m_currentApplyPass).Apply();
    return ErrorCode::ok;
}

bool EffectTechnique::HasNextPass()
{
    if (m_currentApplyPass < m_effectPasses.end() - 1) return true;
    return false;
}

void EffectTechnique::CommitVariables()
{
    if (m_effectPasses.empty()) return;
    for (auto& pass : m_effectPasses)
    {
        pass.CommitVariables();
    }
}
