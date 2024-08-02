#include "SceneRenderingConfiguration.h"
#include <cassert>
#include <string>

using namespace Enigma::Rendering;

SceneRenderingConfiguration::SceneRenderingConfiguration()
{
    m_isPrimary = true;
    m_primaryRendererTechniqueName = "Primary";
}

bool SceneRenderingConfiguration::isPrimary() const
{
    return m_isPrimary;
}

void SceneRenderingConfiguration::isPrimary(bool primary)
{
    m_isPrimary = primary;
}

const std::string& SceneRenderingConfiguration::primaryRendererTechniqueName() const
{
    assert(!m_primaryRendererTechniqueName.empty());
    return m_primaryRendererTechniqueName;
}

void SceneRenderingConfiguration::primaryRendererTechniqueName(const std::string& name)
{
    m_primaryRendererTechniqueName = name;
}

