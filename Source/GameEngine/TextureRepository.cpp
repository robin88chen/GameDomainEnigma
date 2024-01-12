#include "TextureRepository.h"
#include "TextureFactory.h"
#include "TextureStoreMapper.h"
#include "Platforms/MemoryMacro.h"
#include "TextureCommands.h"
#include "TextureEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/QueryDispatcher.h"
#include "TextureQueries.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, TextureRepository, ISystemService);

TextureRepository::TextureRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<TextureStoreMapper>& store_mapper) : ISystemService(srv_manager), m_storeMapper(store_mapper)
{
    m_factory = menew TextureFactory();
    m_needTick = false;
}

TextureRepository::~TextureRepository()
{
    SAFE_DELETE(m_factory);
}

Enigma::Frameworks::ServiceResult TextureRepository::onInit()
{
    assert(m_storeMapper);

    m_removeTexture = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { removeTexture(c); });
    Frameworks::CommandBus::subscribe(typeid(RemoveTexture), m_removeTexture);
    m_putTexture = std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { putTexture(c); });
    Frameworks::CommandBus::subscribe(typeid(PutTexture), m_putTexture);

    m_queryTexture = std::make_shared<Frameworks::QuerySubscriber>([=](const Frameworks::IQueryPtr& q) { queryTexture(q); });
    Frameworks::QueryDispatcher::subscribe(typeid(QueryTexture), m_queryTexture);

    m_storeMapper->connect();

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult TextureRepository::onTerm()
{
    assert(m_storeMapper);
    m_storeMapper->disconnect();

    m_textures.clear();

    Frameworks::CommandBus::unsubscribe(typeid(RemoveTexture), m_removeTexture);
    m_removeTexture = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(PutTexture), m_putTexture);
    m_putTexture = nullptr;

    Frameworks::QueryDispatcher::unsubscribe(typeid(QueryTexture), m_queryTexture);
    m_queryTexture = nullptr;

    return Frameworks::ServiceResult::Complete;
}

bool TextureRepository::hasTexture(const TextureId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_textureMapLock };
    const auto it = m_textures.find(id);
    if (it != m_textures.end()) return true;
    return m_storeMapper->hasTexture(id);
}

std::shared_ptr<Texture> TextureRepository::queryTexture(const TextureId& id)
{
    if (!hasTexture(id)) return nullptr;
    std::lock_guard locker{ m_textureMapLock };
    auto it = m_textures.find(id);
    if (it != m_textures.end()) return it->second;
    assert(m_factory);
    const auto dto = m_storeMapper->queryTexture(id);
    assert(dto.has_value());
    auto tex = m_factory->constitute(id, dto.value(), true);
    assert(tex);
    m_textures.insert_or_assign(id, tex);
    return tex;
}

void TextureRepository::removeTexture(const TextureId& id)
{
    if (!hasTexture(id)) return;
    std::lock_guard locker{ m_textureMapLock };
    m_textures.erase(id);
    error er = m_storeMapper->removeTexture(id);
    if (er)
    {
        Platforms::Debug::ErrorPrintf("remove texture %s failed : %s\n", id.name().c_str(), er.message().c_str());
        Frameworks::EventPublisher::post(std::make_shared<RemoveTextureFailed>(id, er));
    }
}

void TextureRepository::putTexture(const TextureId& id, const std::shared_ptr<Texture>& texture)
{
    if (hasTexture(id)) return;
    std::lock_guard locker{ m_textureMapLock };
    m_textures.insert_or_assign(id, texture);
    error er = m_storeMapper->putTexture(id, texture->serializeDto());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("put primitive %s failed : %s\n", id.name().c_str(), er.message().c_str());
        Frameworks::EventPublisher::post(std::make_shared<PutTextureFailed>(id, er));
    }
}

void TextureRepository::removeTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RemoveTexture>(c);
    if (!cmd) return;
    removeTexture(cmd->id());
}

void TextureRepository::putTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<PutTexture>(c);
    if (!cmd) return;
    putTexture(cmd->id(), cmd->texture());
}

void TextureRepository::queryTexture(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryTexture>(q);
    if (!query) return;
    auto tex = queryTexture(query->id());
    query->setResult(tex);
}
