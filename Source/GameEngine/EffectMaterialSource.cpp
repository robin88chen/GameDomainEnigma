#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include <cassert>

#include "Platforms/MemoryAllocMacro.h"

using namespace Enigma::Engine;

std::function<void(const std::shared_ptr<EffectMaterialSource>&)> EffectMaterialSource::OnDuplicatedEmpty;

EffectMaterialSource::EffectMaterialSource()
{
    m_duplicateCount = 0;
    m_effectMaterial = nullptr;
}

EffectMaterialSource::EffectMaterialSource(std::unique_ptr<EffectMaterial> material)
{
    m_duplicateCount = 1;
    m_effectMaterial = std::move(material);
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

void EffectMaterialSource::LinkSource()
{
    if (m_effectMaterial) m_effectMaterial->SetSource(shared_from_this());
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
