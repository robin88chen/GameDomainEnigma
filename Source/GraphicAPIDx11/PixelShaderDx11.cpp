#include "PixelShaderDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Frameworks/StringFormat.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include <d3dcompiler.h>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

PixelShaderDx11::PixelShaderDx11(const std::string& name) : IPixelShader(name)
{
    m_d3dShader = nullptr;
    m_d3dShaderReflect = nullptr;
    m_semanticTable.clear();
}

PixelShaderDx11::~PixelShaderDx11()
{
    SAFE_RELEASE(m_d3dShader);
    SAFE_RELEASE(m_d3dShaderReflect);
    m_semanticTable.clear();
}

void PixelShaderDx11::ParseSemanticTable(const std::string& code)
{
    m_semanticTable.clear();
#include "ParseSemanticTable.inl"
}
