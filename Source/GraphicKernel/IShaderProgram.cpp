#include "IShaderProgram.h"
#include "IGraphicAPI.h"

using namespace Enigma::Graphics;

IShaderProgram::IShaderProgram(const std::string& name, const IVertexShaderPtr& vtx_shader,
    const IPixelShaderPtr& pix_shader, const IVertexDeclarationPtr& vtx_decl)
{
    m_name = name;
    m_vtxShader = vtx_shader;
    m_pixShader = pix_shader;
    m_vtxDeclaration = vtx_decl;
}

IShaderProgram::~IShaderProgram()
{
    m_vtxShader = nullptr;
    m_pixShader = nullptr;
    m_vtxDeclaration = nullptr;
}

void IShaderProgram::ApplyVariables()
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        AsyncApplyShaderVariables();
    }
    else
    {
        ApplyShaderVariables();
    }
}
