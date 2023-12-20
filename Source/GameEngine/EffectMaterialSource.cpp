#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include <cassert>

#include "Platforms/MemoryAllocMacro.h"

using namespace Enigma::Engine;

std::function<void(const std::shared_ptr<EffectMaterialSource>&)> EffectMaterialSource::onDuplicatedEmpty;

EffectMaterialSource::EffectMaterialSource(const EffectMaterialId& id)
{
    m_id = id;
    m_duplicateCount = 0;
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

void EffectMaterialSource::linkSource()
{
    if (m_sourceEffectMaterial) m_sourceEffectMaterial->setSource(shared_from_this());
}

EffectMaterialPtr EffectMaterialSource::cloneEffectMaterial()
{
    assert(m_sourceEffectMaterial);
    m_duplicateCount++;
    return EffectMaterialPtr(menew EffectMaterial(*(m_sourceEffectMaterial.get())), [=](EffectMaterial* e) { this->duplicatedEffectDeleter(e); });
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
