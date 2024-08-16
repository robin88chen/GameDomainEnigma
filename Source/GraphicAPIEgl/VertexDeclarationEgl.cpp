#include "VertexDeclarationEgl.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

VertexDeclarationEgl::VertexDeclarationEgl(const std::string& name, const std::string& data_vertex_format)
    : IVertexDeclaration(name, data_vertex_format)
{
    m_layouts.clear();
    Graphics::VertexDescription desc = m_dataVertexFormatCode.calculateVertexSize();
    m_vertexSize = desc.totalVertexSize();
    CreateVertexLayout(desc);
}

VertexDeclarationEgl::~VertexDeclarationEgl()
{
    m_layouts.clear();
}

bool VertexDeclarationEgl::IsMatched(const std::string& data_vertex_format, const Graphics::IVertexShaderPtr& vtx_shader)
{
    Graphics::VertexFormatCode data_vertex_code;
    data_vertex_code.fromString(data_vertex_format);
    if (data_vertex_code != m_dataVertexFormatCode) return false;
    return true;
}

void VertexDeclarationEgl::CreateVertexLayout(const Graphics::VertexDescription& vertex_desc)
{
    if (FATAL_LOG_EXPR(vertex_desc.numberOfElements() <= 0)) return;

    m_layouts.clear();
    m_layouts.resize(vertex_desc.numberOfElements());

    unsigned int element_idx = 0;
    if (vertex_desc.positionOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.positionOffset() * sizeof(float));
        m_layouts[element_idx].m_size = vertex_desc.positionDimension();
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.weightOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.weightOffset() * sizeof(float));
        m_layouts[element_idx].m_size = vertex_desc.blendWeightCount();
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.paletteIndexOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.paletteIndexOffset() * sizeof(float));
        m_layouts[element_idx].m_size = 1;
        m_layouts[element_idx].m_type = GL_UNSIGNED_INT;
        element_idx++;
    }
    if (vertex_desc.normalOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.normalOffset() * sizeof(float));
        m_layouts[element_idx].m_size = 3;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.diffuseColorOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.diffuseColorOffset() * sizeof(float));
        if (vertex_desc.diffuseColorDimension() == 1)
        {
            m_layouts[element_idx].m_size = 1;
            m_layouts[element_idx].m_type = GL_UNSIGNED_INT;
        }
        else
        {
            m_layouts[element_idx].m_size = 4;
            m_layouts[element_idx].m_type = GL_FLOAT;
        }
        element_idx++;
    }
    if (vertex_desc.specularColorOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.specularColorOffset() * sizeof(float));
        if (vertex_desc.specularColorDimension() == 1)
        {
            m_layouts[element_idx].m_size = 1;
            m_layouts[element_idx].m_type = GL_UNSIGNED_INT;
        }
        else
        {
            m_layouts[element_idx].m_size = 4;
            m_layouts[element_idx].m_type = GL_FLOAT;
        }
        element_idx++;
    }
    if (vertex_desc.tangentOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.tangentOffset() * sizeof(float));
        m_layouts[element_idx].m_size = vertex_desc.tangentDimension();
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.binormalOffset() >= 0)
    {
        m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.binormalOffset() * sizeof(float));
        m_layouts[element_idx].m_size = 3;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    for (unsigned int ti = 0; ti < Graphics::VertexFormatCode::MAX_TEX_COORD; ti++)
    {
        if (vertex_desc.textureCoordOffset(ti) >= 0)
        {
            m_layouts[element_idx].m_position = static_cast<GLint>(vertex_desc.textureCoordOffset(ti) * sizeof(float));
            m_layouts[element_idx].m_size = vertex_desc.textureCoordSize(ti);
            m_layouts[element_idx].m_type = GL_FLOAT;
            element_idx++;
        }
        else break;
    }
}
