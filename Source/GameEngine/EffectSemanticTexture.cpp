#include "EffectSemanticTexture.h"

using namespace Enigma::Engine;

EffectSemanticTexture::EffectSemanticTexture()
{
    m_texture = nullptr;
}

EffectSemanticTexture::EffectSemanticTexture(const TextureId& texture_id, const std::optional<unsigned int>& arrayIndex, const std::string& semantic)
{
    m_textureId = texture_id;
    m_arrayIndex = arrayIndex;
    m_semantic = semantic;
    m_texture = Texture::queryTexture(texture_id);
}

EffectSemanticTexture::~EffectSemanticTexture()
{
    m_texture = nullptr;
}
