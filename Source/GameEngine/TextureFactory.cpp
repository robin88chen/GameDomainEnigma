#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Texture.h"
#include "TextureFactory.h"
#include "TextureResourceProcessor.h"
#include "Frameworks/CommandBus.h"
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
    m_constituteTexture = std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { constituteTexture(c); });
    Frameworks::CommandBus::subscribe(typeid(ConstituteTexture), m_constituteTexture);
}

void TextureFactory::unregisterHandlers()
{
    Frameworks::CommandBus::unsubscribe(typeid(ConstituteTexture), m_constituteTexture);
    m_constituteTexture = nullptr;
}

std::shared_ptr<Texture> TextureFactory::create(const TextureId& id)
{
    return std::make_shared<Texture>(id);
}

std::shared_ptr<Texture> TextureFactory::constitute(const TextureId& id, const GenericDto& dto, bool is_persisted)
{
    auto texture = std::make_shared<Texture>(id, dto);
    auto er = m_processor->enqueueContentingDto(texture, dto);
    if (er) return nullptr;
    er = m_processor->contentNextTextureResource();
    if (er) return nullptr;
    Frameworks::EventPublisher::post(std::make_shared<TextureConstituted>(id, texture, is_persisted));
    return texture;
}

void TextureFactory::constituteTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ConstituteTexture>(c);
    if (!cmd) return;
    constitute(cmd->id(), cmd->dto(), false);
}

