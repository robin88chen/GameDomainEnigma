#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include <cassert>

#include "Platforms/MemoryAllocMacro.h"

using namespace Enigma::Engine;

EffectMaterialSource::EffectMaterialSource()
{
    m_duplicateCount = 0;
    m_effectMaterial = nullptr;
}

EffectMaterialSource::~EffectMaterialSource()
{
    assert(m_duplicateCount <= 1);
    m_effectMaterial = nullptr;
}

const std::string& EffectMaterialSource::GetName() const
{
    assert(m_effectMaterial != nullptr);
    return m_effectMaterial->GetName();
}

EffectMaterialPtr EffectMaterialSource::CloneEffectMaterial()
{
    assert(m_effectMaterial != nullptr);
    m_duplicateCount++;
    return EffectMaterialPtr(menew EffectMaterial(*(m_effectMaterial.get())), [=](EffectMaterial* e) { this->DuplicatedEffectDeleter(e); });
}

void EffectMaterialSource::DuplicatedEffectDeleter(EffectMaterial* effect)
{
    assert(effect != m_effectMaterial.get());
    if (effect->GetEffectMaterialSource() == shared_from_this())
    {
        m_duplicateCount--;
        assert(m_duplicateCount > 0);
        if (m_duplicateCount <= 1)
        {
            OnDuplicatedEmpty(shared_from_this());
        }
    }
    delete effect;
}
