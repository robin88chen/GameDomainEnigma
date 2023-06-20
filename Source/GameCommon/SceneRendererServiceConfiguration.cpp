#include "SceneRendererServiceConfiguration.h"
#include <cassert>
#include <string>

using namespace Enigma::GameCommon;

SceneRendererServiceConfiguration::SceneRendererServiceConfiguration()
{
    m_isPrimary = true;
    m_primaryRendererTechniqueName = "Primary";
}

bool SceneRendererServiceConfiguration::IsPrimary() const
{
    return m_isPrimary;
}

bool& SceneRendererServiceConfiguration::IsPrimary()
{
    return m_isPrimary;
}

const std::string& SceneRendererServiceConfiguration::PrimaryRendererTechniqueName() const
{
    assert(!m_primaryRendererTechniqueName.empty());
    return m_primaryRendererTechniqueName;
}

std::string& SceneRendererServiceConfiguration::PrimaryRendererTechniqueName()
{
    return m_primaryRendererTechniqueName;
}

