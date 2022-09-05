#include "EffectMaterial.h"

using namespace Enigma::Engine;

EffectMaterial::EffectMaterial(const std::string& source_filename)
{
    m_sourceFileName = source_filename;
    m_effectTechniques.clear();
    m_currentTechnique = m_effectTechniques.end();
}

EffectMaterial::EffectMaterial(const EffectMaterial& eff)
{
    m_sourceFileName = eff.m_sourceFileName;
    m_sourceMaterial = eff.m_sourceMaterial.lock();
    m_effectTechniques = eff.m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();
}

EffectMaterial::EffectMaterial(EffectMaterial&& eff)
{
    m_sourceFileName = std::move(eff.m_sourceFileName);
    m_sourceMaterial = std::move(eff.m_sourceMaterial.lock());
    m_effectTechniques = std::move(eff.m_effectTechniques);
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = std::move(eff.m_instancedAssignFuncList);
    m_selectedRendererTechName = std::move(eff.m_selectedRendererTechName);
    m_selectedVisualTechName = std::move(eff.m_selectedVisualTechName);
}

EffectMaterial& EffectMaterial::operator=(const EffectMaterial& eff)
{
    m_sourceFileName = eff.m_sourceFileName;
    m_sourceMaterial = eff.m_sourceMaterial.lock();
    m_effectTechniques = eff.m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();

    return *this;
}

EffectMaterial& EffectMaterial::operator=(EffectMaterial&& eff)
{
    m_sourceFileName = std::move(eff.m_sourceFileName);
    m_sourceMaterial = std::move(eff.m_sourceMaterial.lock());
    m_effectTechniques = std::move(eff.m_effectTechniques);
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = std::move(eff.m_instancedAssignFuncList);
    m_selectedRendererTechName = std::move(eff.m_selectedRendererTechName);
    m_selectedVisualTechName = std::move(eff.m_selectedVisualTechName);

    return *this;
}

