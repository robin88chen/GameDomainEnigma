#include "EffectMaterialSourceRepository.h"
#include "EffectCompiler.h"
#include "EffectMaterialSourceStoreMapper.h"
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
    m_compiler = menew EffectCompiler();
}

EffectMaterialSourceRepository::~EffectMaterialSourceRepository()
{
    SAFE_DELETE(m_compiler);
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
    m_compiler->compileEffect(profile.value());
    return nullptr;
}
