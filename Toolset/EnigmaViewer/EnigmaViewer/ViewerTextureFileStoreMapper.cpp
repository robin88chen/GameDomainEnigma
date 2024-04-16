#include "ViewerTextureFileStoreMapper.h"
#include "ViewerQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace EnigmaViewer;
using namespace Enigma::FileStorage;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

ViewerTextureFileStoreMapper::ViewerTextureFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<IDtoGateway>& gateway) : TextureFileStoreMapper(mapper_filename, gateway),
m_resolveTextureId(nullptr)
{
}

ViewerTextureFileStoreMapper::~ViewerTextureFileStoreMapper()
{
}

void ViewerTextureFileStoreMapper::subscribeHandlers()
{
    m_resolveTextureId = std::make_shared<QuerySubscriber>([=](auto q) { resolveTextureId(q); });
    QueryDispatcher::subscribe(typeid(ResolveTextureId), m_resolveTextureId);
}

void ViewerTextureFileStoreMapper::unsubscribeHandlers()
{
    QueryDispatcher::unsubscribe(typeid(ResolveTextureId), m_resolveTextureId);
    m_resolveTextureId = nullptr;
}

std::optional<Enigma::Engine::TextureId> ViewerTextureFileStoreMapper::textureId(const std::string& texture_name) const
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

void ViewerTextureFileStoreMapper::resolveTextureId(const Enigma::Frameworks::IQueryPtr& q)
{
    const auto query = std::dynamic_pointer_cast<ResolveTextureId>(q);
    if (query == nullptr) return;

    const auto texture_id = textureId(query->filename());
    if (texture_id) query->setResult(texture_id.value());
}
