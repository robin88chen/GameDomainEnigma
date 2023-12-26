#include "Texture.h"
#include "GraphicKernel/ITexture.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, Texture);

Texture::Texture(const TextureId& id) : m_factoryDesc(Texture::TYPE_RTTI.getName())
{
    m_id = id;
}

Texture::Texture(const TextureId& id, const GenericDto& dto) : m_factoryDesc(Texture::TYPE_RTTI.getName())
{
    m_id = id;
}

Texture::Texture(const std::string& name, const Graphics::ITexturePtr& tex) : m_factoryDesc(Texture::TYPE_RTTI.getName())
{
    m_name = name;
    m_texture = tex;
}

Texture::~Texture()
{
    m_texture = nullptr;
}

const Enigma::MathLib::Dimension<unsigned>& Texture::GetDimension()
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
