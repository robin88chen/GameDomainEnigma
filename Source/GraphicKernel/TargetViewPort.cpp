#include "TargetViewPort.h"
#include "MathLib/MathGlobal.h"

using namespace Enigma::Graphics;

TargetViewPort::TargetViewPort()
{
    m_x = m_y = 0;
    m_width = m_height = 1;
    m_minZ = 0.0f;
    m_maxZ = 1.0f;
}

TargetViewPort::TargetViewPort(unsigned int x, unsigned int y,
    unsigned int width, unsigned int height,
    float min_z/* =0.0f */, float max_z/* =1.0f */)
{
    m_x = x; m_y = y;
    m_width = width; m_height = height;
    m_minZ = min_z; m_maxZ = max_z;
}

TargetViewPort::~TargetViewPort()
{

}

bool TargetViewPort::operator==(const TargetViewPort& vp)
{
    if (m_x != vp.m_x) return false;
    if (m_y != vp.m_y) return false;
    if (m_width != vp.m_width) return false;
    if (m_height != vp.m_height) return false;
    if (!MathLib::Math::IsEqual(m_minZ, vp.m_minZ)) return false;
    if (!MathLib::Math::IsEqual(m_maxZ, vp.m_maxZ)) return false;

    return true;
}
