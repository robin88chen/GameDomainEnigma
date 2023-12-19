#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include <cassert>

#include "Platforms/MemoryAllocMacro.h"

using namespace Enigma::Engine;

std::function<void(const std::shared_ptr<EffectMaterialSource>&)> EffectMaterialSource::onDuplicatedEmpty;

EffectMaterialSource::EffectMaterialSource()
{
    m_duplicateCount = 0;
    m_effectMaterial = nullptr;
}

EffectMaterialSource::EffectMaterialSource(std::shared_ptr<EffectMaterial> material)
{
    m_duplicateCount = 1;
    m_effectMaterial = material;
}

EffectMaterialSource::~EffectMaterialSource()
{
    assert(m_duplicateCount <= 1);
    m_effectMaterial = nullptr;
}

const std::string& EffectMaterialSource::getName() const
{
    assert(m_effectMaterial);
    return m_effectMaterial->getName();
}

void EffectMaterialSource::linkSource()
{
    if (m_effectMaterial) m_effectMaterial->setSource(shared_from_this());
}

EffectMaterialPtr EffectMaterialSource::cloneEffectMaterial()
{
    assert(m_effectMaterial);
    m_duplicateCount++;
    return EffectMaterialPtr(menew EffectMaterial(*(m_effectMaterial.get())), [=](EffectMaterial* e) { this->duplicatedEffectDeleter(e); });
}

void EffectMaterialSource::duplicatedEffectDeleter(EffectMaterial* effect)
{
    assert(effect != m_effectMaterial.get());
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
