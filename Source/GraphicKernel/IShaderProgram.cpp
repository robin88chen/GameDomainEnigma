#include "IShaderProgram.h"

using namespace Enigma::Graphics;

IShaderProgram::IShaderProgram(const std::string& name, const IVertexShaderPtr& vtx_shader,
    const IPixelShaderPtr& pix_shader)
{
    m_name = name;
    m_vtxShader = vtx_shader;
    m_pixShader = pix_shader;
}

IShaderProgram::~IShaderProgram()
{
    m_vtxShader = nullptr;
    m_pixShader = nullptr;
}

