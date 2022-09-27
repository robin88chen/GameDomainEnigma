#include "Texture.h"
#include "GraphicKernel/ITexture.h"
#include <cassert>

using namespace Enigma::Engine;

Texture::Texture(const std::string& name, const Graphics::ITexturePtr & tex)
{
    m_name = name;
    m_texture = tex;
}

Texture::~Texture()
{
    m_texture = nullptr;
}

const Enigma::MathLib::Dimension& Texture::GetDimension()
{
    assert(m_texture);
    return m_texture->GetDimension();
}

const Enigma::Graphics::GraphicFormat& Texture::GetFormat()
{
    assert(m_texture);
    return m_texture->GetFormat();
}

bool Texture::IsCubeTexture()
{
    assert(m_texture);
    return m_texture->IsCubeTexture();
}
