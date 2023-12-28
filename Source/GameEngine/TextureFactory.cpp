#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Texture.h"
#include "TextureFactory.h"
#include "TextureResourceProcessor.h"

using namespace Enigma::Engine;

using error = std::error_code;

TextureFactory::TextureFactory()
{
    m_processor = menew TextureResourceProcessor();
}

TextureFactory::~TextureFactory()
{
    SAFE_DELETE(m_processor);
}

void TextureFactory::registerHandlers()
{

}

void TextureFactory::unregisterHandlers()
{

}

std::shared_ptr<Texture> TextureFactory::create(const TextureId& id)
{
    return std::make_shared<Texture>(id);
}

std::shared_ptr<Texture> TextureFactory::constitute(const TextureId& id, const GenericDto& dto)
{
    auto texture = std::make_shared<Texture>(id, dto);
    auto er = m_processor->enqueueContentingDto(texture, dto);
    if (er) return nullptr;
    er = m_processor->contentNextTextureResource();
    if (er) return nullptr;
    return texture;
}
