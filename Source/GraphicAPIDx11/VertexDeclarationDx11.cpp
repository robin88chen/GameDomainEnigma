#include "VertexDeclarationDx11.h"
#include "VertexShaderDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

const std::string VertexDeclarationDx11::m_positionSemanticName = "POSITION";
const std::string VertexDeclarationDx11::m_normalSemanticName = "NORMAL";
const std::string VertexDeclarationDx11::m_color0SemanticName = "DIFFUSE";
const std::string VertexDeclarationDx11::m_color1SemanticName = "SPECULAR";
const std::string VertexDeclarationDx11::m_texCoordSemanticName = "TEXCOORD";
const std::string VertexDeclarationDx11::m_weightsSemanticName = "BLENDWEIGHT";
const std::string VertexDeclarationDx11::m_boneIndexSemanticName = "BLENDINDICES";
const std::string VertexDeclarationDx11::m_tangentSemanticName = "TANGENT";
const std::string VertexDeclarationDx11::m_binormalSemanticName = "BINORMAL";

VertexDeclarationDx11::VertexDeclarationDx11(const std::string& name, const std::string& data_vertex_format,
    const Graphics::VertexFormatCode& shader_fmt_code) : IVertexDeclaration(name, data_vertex_format)
{
    m_d3dInputLayout = nullptr;
    m_shaderVertexFormat = shader_fmt_code;
}

VertexDeclarationDx11::~VertexDeclarationDx11()
{
    SAFE_RELEASE(m_d3dInputLayout);
}

bool VertexDeclarationDx11::IsMatched(const std::string& data_vertex_format, const Graphics::IVertexShaderPtr& vtx_shader)
{
    Graphics::VertexFormatCode data_vertex_code;
    data_vertex_code.FromString(data_vertex_format);
    if (data_vertex_code != m_dataVertexFormatCode) return false;
    VertexShaderDx11* shader_dx11 = dynamic_cast<VertexShaderDx11*>(vtx_shader.get());
    assert(shader_dx11);
    if (m_shaderVertexFormat != shader_dx11->GetShaderVertexFormat()) return false;
    return true;
}

error VertexDeclarationDx11::CreateD3DInputLayout(ID3D11Device* device)
{
    assert(device);

    if (FATAL_LOG_EXPR(m_shaderSignatureBytes.empty())) return ErrorCode::shaderSignature;

    D3D11_INPUT_ELEMENT_DESC* input_layout = nullptr;
    unsigned int num_element = 0;
    std::tie(input_layout, num_element) = CreateVertexLayout(m_dataVertexFormatDesc);
    if (FATAL_LOG_EXPR((!input_layout) || (num_element == 0))) return ErrorCode::vertexLayout;
    HRESULT hr = device->CreateInputLayout(input_layout, num_element, &m_shaderSignatureBytes[0], m_shaderSignatureBytes.size(), &m_d3dInputLayout);
    mefree(input_layout);

    if (FAILED(hr))
    {
        return ErrorCode::dxCreateInputLayout;
    }

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceInputLayoutCreated>(m_name));
    return ErrorCode::ok;
}

void VertexDeclarationDx11::FillShaderVertexFormat(const std::shared_ptr<VertexShaderDx11>& shader)
{
    assert(shader);
    m_shaderVertexFormat = shader->GetShaderVertexFormat();
    m_shaderSignatureBytes = shader->GetShaderSignatureBytes();
}

std::tuple<D3D11_INPUT_ELEMENT_DESC*, unsigned int> VertexDeclarationDx11::CreateVertexLayout(const Graphics::VertexDescription& vertex_desc)
{
    if (vertex_desc.NumberOfElements() <= 0) return { nullptr, 0 };

    D3D11_INPUT_ELEMENT_DESC* input_layout = memalloc(D3D11_INPUT_ELEMENT_DESC, vertex_desc.NumberOfElements());
    memset(input_layout, 0, vertex_desc.NumberOfElements() * sizeof(D3D11_INPUT_ELEMENT_DESC));

    unsigned int element_idx = 0;
    if (vertex_desc.PositionOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_positionSemanticName.c_str();
        input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.PositionOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.WeightOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_weightsSemanticName.c_str();
        switch (vertex_desc.BlendWeightCount())
        {
        case 1:
            input_layout[element_idx].Format = DXGI_FORMAT_R32_FLOAT;
            break;
        case 2:
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case 3:
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case 4:
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
            default:
                assert(false);
        }
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.WeightOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.PaletteIndexOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_boneIndexSemanticName.c_str();
        input_layout[element_idx].Format = DXGI_FORMAT_R32_UINT;
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.PaletteIndexOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.NormalOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_normalSemanticName.c_str();
        input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.NormalOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.DiffuseColorOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_color0SemanticName.c_str();
        if (vertex_desc.DiffuseColorDimension() == 1)
        {
            input_layout[element_idx].Format = DXGI_FORMAT_R32_UINT;
        }
        else
        {
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.DiffuseColorOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.SpecularColorOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_color1SemanticName.c_str();
        if (vertex_desc.SpecularColorDimension() == 1)
        {
            input_layout[element_idx].Format = DXGI_FORMAT_R32_UINT;
        }
        else
        {
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.SpecularColorOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.TangentOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_tangentSemanticName.c_str();
        if (vertex_desc.TangentDimension() == 3)
        {
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else
        {
            input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.TangentOffset() * sizeof(float);

        element_idx++;
    }
    if (vertex_desc.BiNormalOffset() >= 0)
    {
        input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_binormalSemanticName.c_str();
        input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        input_layout[element_idx].InputSlot = 0;
        input_layout[element_idx].AlignedByteOffset = vertex_desc.BiNormalOffset() * sizeof(float);

        element_idx++;
    }
    for (unsigned int ti = 0; ti < Graphics::VertexFormatCode::MAX_TEX_COORD; ti++)
    {
        if (vertex_desc.TextureCoordOffset(ti) >= 0)
        {
            input_layout[element_idx].SemanticName = VertexDeclarationDx11::m_texCoordSemanticName.c_str();
            input_layout[element_idx].SemanticIndex = ti;
            switch (vertex_desc.TextureCoordSize(ti))
            {
            case 1:
                input_layout[element_idx].Format = DXGI_FORMAT_R32_FLOAT;
                break;
            case 2:
                input_layout[element_idx].Format = DXGI_FORMAT_R32G32_FLOAT;
                break;
            case 3:
                input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32_FLOAT;
                break;
            case 4:
                input_layout[element_idx].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                break;
            }
            input_layout[element_idx].InputSlot = 0;
            input_layout[element_idx].AlignedByteOffset = vertex_desc.TextureCoordOffset(ti) * sizeof(float);

            element_idx++;
        }
        else break;
    }

    return { input_layout, vertex_desc.NumberOfElements() };
}

