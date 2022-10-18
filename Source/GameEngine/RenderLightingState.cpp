#include "RenderLightingState.h"

#include "MaterialVariableMap.h"

using namespace Enigma::Engine;

RenderLightingState::RenderLightingState()
{
    m_colorAmbient = MathLib::ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    m_colorSun = MathLib::ColorRGBA::BLACK;
    m_vecSunDir = MathLib::Vector3::UNIT_Y;
}

RenderLightingState::~RenderLightingState()
{
}

void RenderLightingState::SetAmbientLightColor(const MathLib::ColorRGBA& color)
{
    m_colorAmbient = color;
}

void RenderLightingState::SetSunLight(const MathLib::Vector3& dir, const MathLib::ColorRGBA& color)
{
    m_vecSunDir = dir;
    m_colorSun = color;
}

void RenderLightingState::SetPointLightArray(const std::vector<MathLib::Vector4>& positions,
    std::vector<MathLib::ColorRGBA>& colors, std::vector<MathLib::Vector4>& attenuations)
{
    m_lightPositions = positions;
    m_lightColors = colors;
    m_lightAttenuations = attenuations;
}

void RenderLightingState::CommitState() const
{
    MaterialVariableMap::UseAmbientLightColor(m_colorAmbient);
    MaterialVariableMap::UseSunLight(m_colorSun, m_vecSunDir);
    if (m_lightPositions.size() > 0)
    {
        MaterialVariableMap::UsePointLight(
            const_cast<MathLib::Vector4*>(&m_lightPositions[0]), const_cast<MathLib::ColorRGBA*>(&m_lightColors[0]),
            const_cast<MathLib::Vector4*>(&m_lightAttenuations[0]), static_cast<unsigned int>(m_lightPositions.size()));
    }
}
