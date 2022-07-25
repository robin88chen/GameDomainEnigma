#include "VertexDeclarationEgl.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

VertexDeclarationEgl::VertexDeclarationEgl(const std::string& name, const std::string& data_vertex_format)
        : IVertexDeclaration(name, data_vertex_format)
{
    m_layouts.clear();
    VertexDesc desc;
    m_vertexSize = m_dataVertexFormatCode.CalculateVertexSize(&desc);
    CreateVertexLayout(desc);
}

VertexDeclarationEgl::~VertexDeclarationEgl()
{
    m_layouts.clear();
}

bool VertexDeclarationEgl::IsMatched(const std::string& data_vertex_format, const Graphics::IVertexShaderPtr& vtx_shader)
{
    VertexFormatCode data_vertex_code;
    data_vertex_code.FromString(data_vertex_format);
    if (data_vertex_code != m_dataVertexFormatCode) return false;
    return true;
}

void VertexDeclarationEgl::CreateVertexLayout(const VertexDesc& vertex_desc)
{
    if (FATAL_LOG_EXPR(vertex_desc.m_numElements <= 0)) return;

    m_layouts.clear();
    m_layouts.resize(vertex_desc.m_numElements);

    unsigned int element_idx = 0;
    if (vertex_desc.m_positionOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_positionOffset * sizeof(float);
        m_layouts[element_idx].m_size = vertex_desc.m_posVecDimension;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.m_weightOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_weightOffset * sizeof(float);
        m_layouts[element_idx].m_size = vertex_desc.m_blendWeightCount;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.m_paletteIndexOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_paletteIndexOffset * sizeof(float);
        m_layouts[element_idx].m_size = 1;
        m_layouts[element_idx].m_type = GL_UNSIGNED_INT;
        element_idx++;
    }
    if (vertex_desc.m_normalOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_normalOffset * sizeof(float);
        m_layouts[element_idx].m_size = 3;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.m_colorOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_colorOffset * sizeof(float);
        if (vertex_desc.m_colorDimension == 1)
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
    if (vertex_desc.m_specularOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_specularOffset * sizeof(float);
        if (vertex_desc.m_specularDimension == 1)
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
    if (vertex_desc.m_tangentOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_tangentOffset * sizeof(float);
        m_layouts[element_idx].m_size = vertex_desc.m_tangentDimension;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    if (vertex_desc.m_biNormalOffset >= 0)
    {
        m_layouts[element_idx].m_position = vertex_desc.m_biNormalOffset * sizeof(float);
        m_layouts[element_idx].m_size = 3;
        m_layouts[element_idx].m_type = GL_FLOAT;
        element_idx++;
    }
    for (unsigned int ti = 0; ti < VertexFormatCode::MAX_TEX_COORD; ti++)
    {
        if (vertex_desc.m_texCoordOffset[ti] >= 0)
        {
            m_layouts[element_idx].m_position = vertex_desc.m_texCoordOffset[ti] * sizeof(float);
            m_layouts[element_idx].m_size = vertex_desc.m_texCoordSize[ti];
            m_layouts[element_idx].m_type = GL_FLOAT;
            element_idx++;
        }
        else break;
    }
}
