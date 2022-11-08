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
    Graphics::VertexDescription desc = m_dataVertexFormatCode.CalculateVertexSize();
    m_vertexSize = desc.TotalVertexSize();
    CreateVertexLayout(desc);
}

VertexDeclarationEgl::~VertexDeclarationEgl()
{
    m_layouts.clear();
}

bool VertexDeclarationEgl::IsMatched(const std::string& data_vertex_format, const Graphics::IVertexShaderPtr& vtx_shader)
{
    Graphics::VertexFormatCode data_vertex_code;
    data_vertex_code.FromString(data_vertex_format);
    if (data_vertex_code != m_dataVertexFormatCode) return false;
    return true;
}

void VertexDeclarationEgl::CreateVertexLayout(const Graphics::VertexDescription& vertex_desc)
{
    if (FATAL_LOG_EXPR(vertex_desc.NumberOfElements() <= 0)) return;

    m_layouts.clear();
    m_layouts.resize(vertex_desc.NumberOfElements());

    unsigned int element_idx = 0;
    if (vertex_desc.PositionOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.PositionOffset() * sizeof(float);
        m_layouts[element_idx].m_size = vertex_desc.PositionDimension();
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.WeightOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.WeightOffset() * sizeof(float);
        m_layouts[element_idx].m_size = vertex_desc.BlendWeightCount();
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.PaletteIndexOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.PaletteIndexOffset() * sizeof(float);
        m_layouts[element_idx].m_size = 1;
        m_layouts[element_idx].m_type = GL_UNSIGNED_INT;
        element_idx++;
    }
    if (vertex_desc.NormalOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.NormalOffset() * sizeof(float);
        m_layouts[element_idx].m_size = 3;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.DiffuseColorOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.DiffuseColorOffset() * sizeof(float);
        if (vertex_desc.DiffuseColorDimension() == 1)
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
    if (vertex_desc.SpecularColorOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.SpecularColorOffset() * sizeof(float);
        if (vertex_desc.SpecularColorDimension() == 1)
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
    if (vertex_desc.TangentOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.TangentOffset() * sizeof(float);
        m_layouts[element_idx].m_size = vertex_desc.TangentDimension();
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.BiNormalOffset() >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.BiNormalOffset() * sizeof(float);
        m_layouts[element_idx].m_size = 3;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    for (unsigned int ti = 0; ti < Graphics::VertexFormatCode::MAX_TEX_COORD; ti++)
    {
        if (vertex_desc.TextureCoordOffset(ti) >= 0)
        {
            m_layouts[element_idx].m_position = vertex_desc.TextureCoordOffset(ti) * sizeof(float);
            m_layouts[element_idx].m_size = vertex_desc.TextureCoordSize(ti);
            m_layouts[element_idx].m_type = GL_FLOAT;
            element_idx++;
        }
        else break;
    }
}
