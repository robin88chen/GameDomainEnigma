#include "TextureFileStoreMapper.h"
#include "LevelEditorQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

TextureFileStoreMapper::TextureFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::TextureFileStoreMapper(mapper_filename, gateway)
{
}

TextureFileStoreMapper::~TextureFileStoreMapper()
{
}

void TextureFileStoreMapper::subscribeHandlers()
{
    m_resolveTextureId = std::make_shared<QuerySubscriber>([=](auto q) { resolveTextureId(q); });
    QueryDispatcher::subscribe(typeid(ResolveTextureId), m_resolveTextureId);
}

void TextureFileStoreMapper::unsubscribeHandlers()
{
    QueryDispatcher::unsubscribe(typeid(ResolveTextureId), m_resolveTextureId);
    m_resolveTextureId = nullptr;
}

std::optional<Enigma::Engine::TextureId> TextureFileStoreMapper::textureId(const std::string& texture_name) const
{
    std::lock_guard lock(m_fileMapLock);
    for (const auto& [id, name] : m_filename_map)
    {
        if (id.name() == texture_name)
        {
            return id;
        }
    }
    return std::nullopt;
}

void TextureFileStoreMapper::resolveTextureId(const Enigma::Frameworks::IQueryPtr& q)
{
    const auto query = std::dynamic_pointer_cast<ResolveTextureId>(q);
    if (query == nullptr) return;

    const auto texture_id = textureId(query->filename());
    if (texture_id) query->setResult(texture_id.value());
}
