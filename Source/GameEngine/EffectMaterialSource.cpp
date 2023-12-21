#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Engine;

std::function<void(const std::shared_ptr<EffectMaterialSource>&)> EffectMaterialSource::onDuplicatedEmpty;

EffectMaterialSource::EffectMaterialSource(const EffectMaterialId& id)
{
    m_id = id;
    m_duplicateCount = 0;
    m_duplicatedSerial = 0;
    m_sourceEffectMaterial = std::make_shared<EffectMaterial>(id);
}

EffectMaterialSource::EffectMaterialSource(std::shared_ptr<EffectMaterial> material)
{
    m_duplicateCount = 1;
    m_sourceEffectMaterial = material;
}

EffectMaterialSource::~EffectMaterialSource()
{
    assert(m_duplicateCount <= 1);
    m_sourceEffectMaterial = nullptr;
}

void EffectMaterialSource::linkSourceSelf()
{
    if (m_sourceEffectMaterial) m_sourceEffectMaterial->setSource(shared_from_this());
}

std::shared_ptr<EffectMaterial> EffectMaterialSource::cloneEffectMaterial()
{
    assert(m_sourceEffectMaterial);
    m_duplicateCount++;
    return EffectMaterialPtr(menew EffectMaterial(*(m_sourceEffectMaterial.get())), [=](EffectMaterial* e) { this->duplicatedEffectDeleter(e); });
}

std::shared_ptr<EffectMaterial> EffectMaterialSource::duplicateEffectMaterial()
{
    assert(m_sourceEffectMaterial);
    m_duplicateCount++;
    m_duplicatedSerial++;
    auto effect = std::shared_ptr<EffectMaterial>(menew EffectMaterial(EffectMaterialId(m_id, m_duplicatedSerial)), [=](EffectMaterial* e) { this->duplicatedEffectDeleter(e); });
    m_duplicatedEffects.emplace_back(effect);
    if (m_sourceEffectMaterial->lazyStatus().isReady())
    {
        effect->copyFrom(m_sourceEffectMaterial);
        effect->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::Ready);
    }
    return effect;
}

void EffectMaterialSource::duplicatedEffectDeleter(EffectMaterial* effect)
{
    assert(effect != m_sourceEffectMaterial.get());
    if (effect->getEffectMaterialSource() == shared_from_this())
    {
        m_duplicateCount--;
        assert(m_duplicateCount > 0);
        if (m_duplicateCount <= 1)
        {
            onDuplicatedEmpty(shared_from_this());
        }
    }
    delete effect;
}
