#include "EffectMaterialSource.h"
#include "EffectMaterial.h"
#include <cassert>

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
