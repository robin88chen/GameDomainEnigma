﻿#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include "EffectEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "EffectCommands.h"
#include "Platforms/PlatformLayerUtilities.h"
#include <cassert>

using namespace Enigma::Engine;

EffectMaterialSource::EffectMaterialSource(const EffectMaterialId& id)
{
    m_id = id;
    m_duplicateCount = 0;
    m_duplicatedSerial = 0;
    m_sourceEffectMaterial = std::make_shared<EffectMaterial>(id);
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

std::shared_ptr<EffectMaterial> EffectMaterialSource::duplicateEffectMaterial()
{
    assert(m_sourceEffectMaterial);
    m_duplicateCount++;
    m_duplicatedSerial++;
    auto effect = std::shared_ptr<EffectMaterial>(menew EffectMaterial(EffectMaterialId(m_id, m_duplicatedSerial)), [=](EffectMaterial* e) { this->duplicatedEffectDeleter(e); });
    effect->setSource(shared_from_this());
    m_duplicatedEffects.emplace_back(effect);
    if (m_sourceEffectMaterial->lazyStatus().isReady())
    {
        effect->copyFrom(m_sourceEffectMaterial);
        effect->lazyStatus().changeStatus(Frameworks::LazyStatus::Status::Ready);
    }
    return effect;
}

void EffectMaterialSource::hydrateDuplicatedEffects()
{
    assert(m_sourceEffectMaterial);
    for (auto& effect : m_duplicatedEffects)
    {
        if (!effect.expired() && !effect.lock()->lazyStatus().isReady())
        {
            effect.lock()->hydrateTechniques(m_sourceEffectMaterial->effectTechniques());
            Frameworks::EventPublisher::enqueue(std::make_shared<EffectMaterialHydrated>(m_id, effect.lock()->id()));
        }
    }
}

std::shared_ptr<EffectMaterial> EffectMaterialSource::queryDuplicatedEffect(const EffectMaterialId& id)
{
    assert(m_id.isEqualSource(id));
    assert(m_sourceEffectMaterial);
    for (auto& effect : m_duplicatedEffects)
    {
        if (auto locked = effect.lock())
        {
            if (locked->id() == id) return locked;
        }
    }
    return nullptr;
}

void EffectMaterialSource::duplicatedEffectDeleter(EffectMaterial* effect)
{
    assert(effect != m_sourceEffectMaterial.get());
    if (effect->getEffectMaterialSource() == shared_from_this())
    {
        assert(m_duplicateCount > 0);
        m_duplicateCount--;
    }
    delete effect;
    if (m_duplicateCount == 0)
    {
        //! ADR : shader 比較耗時，先不做釋放, not test yet
        Platforms::Debug::Printf("Effect Material %s has zero duplication", m_id.name().c_str());
        //std::make_shared<ReleaseEffectMaterial>(m_id)->enqueue();
    }
}
