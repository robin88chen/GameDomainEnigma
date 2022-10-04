#include "RenderLightingState.h"

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
