#include "IMultiBackSurface.h"

using namespace Enigma::Graphics;

IMultiBackSurface::IMultiBackSurface(const std::string& name) : IBackSurface(name, false)
{
    m_surfaceCount = 0;
}

IMultiBackSurface::~IMultiBackSurface()
{
}

GraphicFormat IMultiBackSurface::GetSliceFormat(unsigned int index)
{
    if ((!m_formatArray.empty()) && (index < m_surfaceCount)) return m_formatArray[index];
    return GraphicFormat();
}

unsigned int IMultiBackSurface::GetSurfaceCount()
{
    return m_surfaceCount;
}
