#include "IVertexDeclaration.h"
#include "Frameworks/StringFormat.h"

using namespace Enigma::Graphics;

IVertexDeclaration::IVertexDeclaration(const std::string& name, const std::string& vertex_format)
{
    m_name = name;
    m_formatString = vertex_format;
    m_dataVertexFormatCode.fromString(vertex_format);
    m_dataVertexFormatDesc = m_dataVertexFormatCode.calculateVertexSize();
}

IVertexDeclaration::~IVertexDeclaration()
{
}

