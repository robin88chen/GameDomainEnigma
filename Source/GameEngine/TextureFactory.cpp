#include "TextureFactory.h"
#include "Texture.h"

using namespace Enigma::Engine;

TextureFactory::TextureFactory()
{

}

TextureFactory::~TextureFactory()
{

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
    return std::make_shared<Texture>(id, dto);
}
