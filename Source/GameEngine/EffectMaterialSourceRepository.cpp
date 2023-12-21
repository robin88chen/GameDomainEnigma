#include "EffectMaterialSourceRepository.h"
#include "EffectCompilingQueue.h"
#include "EffectMaterialSourceStoreMapper.h"
#include "EffectMaterialSource.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include <system_error>
#include <cassert>

#include "EffectMaterialSource.h"


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

std::shared_ptr<EffectMaterial> EffectMaterialSourceRepository::duplicateEffectMaterial(const EffectMaterialId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_sourceMapLock };
    const auto it = m_sourceMaterials.find(id);
    if (it != m_sourceMaterials.end()) return it->second->cloneEffectMaterial();
    auto profile = m_storeMapper->queryEffectMaterial(id);
    assert(profile);
    auto source = std::make_shared<EffectMaterialSource>(id);
    source->linkSourceSelf();
    auto er = m_compilingQueue->enqueue(source->self(), profile.value());
    if (er) return nullptr;
    er = m_compilingQueue->compileNextEffect();
    if (er) return nullptr;
    m_sourceMaterials.emplace(id, source);
    return source->duplicateEffectMaterial();
}
