#include "EffectMaterialSourceRepository.h"
#include "EffectCompilingQueue.h"
#include "EffectMaterialSourceStoreMapper.h"
#include "EffectMaterialSource.h"
#include "EffectQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/CommandBus.h"
#include <system_error>
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, EffectMaterialSourceRepository, ISystemService);

using error = std::error_code;

EffectMaterialSourceRepository::EffectMaterialSourceRepository(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<EffectMaterialSourceStoreMapper>& store_mapper) : ISystemService(srv_mngr), m_storeMapper(store_mapper)
{
    m_compilingQueue = menew EffectCompilingQueue();
}

EffectMaterialSourceRepository::~EffectMaterialSourceRepository()
{
    SAFE_DELETE(m_compilingQueue);
}

ServiceResult EffectMaterialSourceRepository::onInit()
{
    assert(m_storeMapper);
    m_storeMapper->connect();

    m_queryEffectMaterial = std::make_shared<QuerySubscriber>([=](const Frameworks::IQueryPtr& q) { queryEffectMaterial(q); });
    QueryDispatcher::subscribe(typeid(QueryEffectMaterial), m_queryEffectMaterial);

    return ServiceResult::Complete;
}

ServiceResult EffectMaterialSourceRepository::onTick()
{
    return ServiceResult::Pendding;
}

ServiceResult EffectMaterialSourceRepository::onTerm()
{
    assert(m_storeMapper);
    m_storeMapper->disconnect();
    m_sourceMaterials.clear();

    QueryDispatcher::unsubscribe(typeid(QueryEffectMaterial), m_queryEffectMaterial);
    m_queryEffectMaterial = nullptr;

    return ServiceResult::Complete;
}

bool EffectMaterialSourceRepository::hasEffectMaterial(const EffectMaterialId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_sourceMapLock };
    const auto it = m_sourceMaterials.find(id);
    if (it != m_sourceMaterials.end()) return true;
    return m_storeMapper->hasEffectMaterial(id);
}

std::shared_ptr<EffectMaterial> EffectMaterialSourceRepository::queryEffectMaterial(const EffectMaterialId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_sourceMapLock };
    const auto it = m_sourceMaterials.find(id.source());
    if (it != m_sourceMaterials.end())
    {
        if (auto eff = it->second->queryDuplicatedEffect(id)) return eff;
        return it->second->duplicateEffectMaterial();
    }
    auto profile = m_storeMapper->queryEffectMaterial(id.source());
    assert(profile);
    auto source = std::make_shared<EffectMaterialSource>(id.source());
    source->linkSourceSelf();
    auto er = m_compilingQueue->enqueue(source->self(), profile.value());
    if (er) return nullptr;
    er = m_compilingQueue->compileNextEffect();
    if (er) return nullptr;
    m_sourceMaterials.emplace(id.source(), source);
    return source->duplicateEffectMaterial();
}

void EffectMaterialSourceRepository::queryEffectMaterial(const Frameworks::IQueryPtr& q)
{
    assert(m_storeMapper);
    auto query = std::dynamic_pointer_cast<QueryEffectMaterial, IQuery>(q);
    assert(query);
    query->setResult(queryEffectMaterial(query->id()));
}

