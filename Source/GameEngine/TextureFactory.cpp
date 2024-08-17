#include "Platforms/MemoryMacro.h"
#include "Texture.h"
#include "TextureFactory.h"
#include "TextureResourceProcessor.h"
#include "TextureCommands.h"
#include "Frameworks/EventPublisher.h"
#include "TextureEvents.h"

using namespace Enigma::Engine;

using error = std::error_code;

TextureFactory::TextureFactory()
{
    m_processor = menew TextureResourceProcessor();
    registerHandlers();
}

TextureFactory::~TextureFactory()
{
    unregisterHandlers();
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

std::shared_ptr<Texture> TextureFactory::constitute(const TextureId& id, const GenericDto& dto, bool is_persisted)
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(id);
    std::shared_ptr<TextureDisassembler> disassembler = std::make_shared<TextureDisassembler>();
    disassembler->disassemble(dto);
    texture->disassemble(disassembler);
    auto er = m_processor->enqueueHydratingDisassembler(texture, disassembler);
    if (er) return nullptr;
    er = m_processor->hydrateNextTextureResource();
    if (er) return nullptr;
    Frameworks::EventPublisher::enqueue(std::make_shared<TextureConstituted>(id, texture, is_persisted));
    return texture;
}
