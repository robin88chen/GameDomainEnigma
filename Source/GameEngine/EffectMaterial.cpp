#include "EffectMaterial.h"
#include "EngineErrors.h"
#include "Frameworks/Rtti.h"
#include "EffectMaterialDto.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(Engine, EffectMaterial);

EffectMaterial::EffectMaterial(const EffectMaterialId& id) : m_factoryDesc(EffectMaterial::TYPE_RTTI.getName())
{
    m_id = id;
    m_currentTechnique = m_effectTechniques.end();
}

EffectMaterial::EffectMaterial(const std::string& name, const std::vector<EffectTechnique>& techniques)
    : m_factoryDesc(EffectMaterial::TYPE_RTTI.getName())
{
    m_factoryDesc.ClaimAsNative(name);
    //m_name = name;
    m_effectTechniques = techniques;
    m_currentTechnique = m_effectTechniques.end();
    mappingAutoVariables();
}

EffectMaterial::EffectMaterial(const EffectMaterial& eff) : m_factoryDesc(eff.m_factoryDesc)
{
    //m_name = eff.m_name;
    m_sourceMaterial = eff.m_sourceMaterial.lock();
    m_effectTechniques = eff.m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();
    mappingAutoVariables();
}

EffectMaterial::EffectMaterial(EffectMaterial&& eff) noexcept : m_factoryDesc(eff.factoryDesc())
{
    //m_name = std::move(eff.m_name);
    m_sourceMaterial = std::move(eff.m_sourceMaterial.lock());
    m_effectTechniques = std::move(eff.m_effectTechniques);
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = std::move(eff.m_instancedAssignFuncList);
    m_selectedRendererTechName = std::move(eff.m_selectedRendererTechName);
    m_selectedVisualTechName = std::move(eff.m_selectedVisualTechName);
    mappingAutoVariables();
}

EffectMaterial::~EffectMaterial()
{
    m_effectTechniques.clear();
    m_instancedAssignFuncList.clear();
}

EffectMaterial& EffectMaterial::operator=(const EffectMaterial& eff)
{
    if (this == &eff) return *this;
    m_factoryDesc = eff.m_factoryDesc;
    //m_name = eff.m_name;
    m_sourceMaterial = eff.m_sourceMaterial.lock();
    m_effectTechniques = eff.m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();
    mappingAutoVariables();

    return *this;
}

EffectMaterial& EffectMaterial::operator=(EffectMaterial&& eff) noexcept
{
    m_factoryDesc = std::move(eff.m_factoryDesc);
    //m_name = std::move(eff.m_name);
    m_sourceMaterial = std::move(eff.m_sourceMaterial.lock());
    m_effectTechniques = std::move(eff.m_effectTechniques);
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = std::move(eff.m_instancedAssignFuncList);
    m_selectedRendererTechName = std::move(eff.m_selectedRendererTechName);
    m_selectedVisualTechName = std::move(eff.m_selectedVisualTechName);
    mappingAutoVariables();

    return *this;
}

GenericDto EffectMaterial::serializeDto()
{
    EffectMaterialDto dto;
    dto.id() = m_id;
    dto.factoryDesc() = m_factoryDesc;
    return dto.toGenericDto();
}

void EffectMaterial::copyFrom(const std::shared_ptr<EffectMaterial>& other)
{
    if (other == nullptr) return;
    m_factoryDesc = other->factoryDesc();
    //m_name = other->m_name;
    m_sourceMaterial = other->m_sourceMaterial.lock();
    m_effectTechniques = other->m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = other->m_instancedAssignFuncList;
    m_selectedRendererTechName = other->m_selectedRendererTechName;
    m_selectedVisualTechName = other->m_selectedVisualTechName;
    mappingAutoVariables();
}

void EffectMaterial::setSource(const std::shared_ptr<EffectMaterialSource>& mat_source)
{
    m_sourceMaterial = mat_source;
}

void EffectMaterial::selectRendererTechnique(const std::string& renderer_tech_name)
{
    m_selectedRendererTechName = renderer_tech_name;
    selectTechnique();
}

void EffectMaterial::selectVisualTechnique(const std::string& visual_tech_name)
{
    m_selectedVisualTechName = visual_tech_name;
    selectTechnique();
}

unsigned int EffectMaterial::getPassCount()
{
    if (m_currentTechnique == m_effectTechniques.end()) return 0;
    return m_currentTechnique->getPassCount();
}

error EffectMaterial::applyFirstPass()
{
    if (m_currentTechnique == m_effectTechniques.end()) return ErrorCode::noSelectedTechnique;
    return m_currentTechnique->applyFirstPass();
}

error EffectMaterial::applyNextPass()
{
    if (m_currentTechnique == m_effectTechniques.end()) return ErrorCode::noSelectedTechnique;
    return m_currentTechnique->applyNextPass();
}

bool EffectMaterial::hasNextPass()
{
    if (m_currentTechnique == m_effectTechniques.end()) return false;
    return m_currentTechnique->hasNextPass();
}

void EffectMaterial::commitInstancedEffectVariables()
{
    if (m_instancedAssignFuncList.size() > 0)
    {
        for (auto& func : m_instancedAssignFuncList)
        {
            func.second(func.first.get());
        }
    }
}

std::list<std::reference_wrapper<EffectVariable>> EffectMaterial::getEffectVariablesByName(const std::string& name)
{
    std::list<std::reference_wrapper<EffectVariable>> result_list;
    result_list.clear();
    if (m_effectTechniques.empty()) return result_list;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.getPassCount(); i++)
        {
            if (auto eff_var = tech.getPassByIndex(i).getVariableByName(name))
                result_list.emplace_back(eff_var.value());
        }
    }
    return result_list;
}

std::list<std::reference_wrapper<EffectVariable>> EffectMaterial::getEffectVariablesBySemantic(const std::string& semantic)
{
    std::list<std::reference_wrapper<EffectVariable>> result_list;
    result_list.clear();
    if (m_effectTechniques.empty()) return result_list;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.getPassCount(); i++)
        {
            if (auto eff_var = tech.getPassByIndex(i).getVariableBySemantic(semantic))
                result_list.emplace_back(eff_var.value());
        }
    }
    return result_list;
}

stdext::optional_ref<EffectVariable> EffectMaterial::getEffectVariableInPassByName(const std::string& pass_name, const std::string& name)
{
    if (m_effectTechniques.empty()) return std::nullopt;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.getPassCount(); i++)
        {
            if (tech.getPassByIndex(i).name() == pass_name)
                return tech.getPassByIndex(i).getVariableByName(name);
        }
    }
    return std::nullopt;
}

stdext::optional_ref<EffectVariable> EffectMaterial::getEffectVariableInPassBySemantic(const std::string& pass_name, const std::string& semantic)
{
    if (m_effectTechniques.empty()) return std::nullopt;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.getPassCount(); i++)
        {
            if (tech.getPassByIndex(i).name() == pass_name)
                return tech.getPassByIndex(i).getVariableBySemantic(semantic);
        }
    }
    return std::nullopt;
}

void EffectMaterial::setVariableAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn)
{
    auto vars_list = getEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        var.get().setValueAssignFunction(fn);
    }
}

void EffectMaterial::setInstancedAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn)
{
    auto vars_list = getEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        auto iter = std::find_if(m_instancedAssignFuncList.begin(), m_instancedAssignFuncList.end(),
            [=](auto& p) -> bool { return p.first.get() == var.get(); });
        if (iter != m_instancedAssignFuncList.end())
        {
            iter->second = fn;
        }
        else
        {
            m_instancedAssignFuncList.emplace_back(std::make_pair(var, fn));
        }
    }
}

void EffectMaterial::assignVariableValue(const std::string& semantic, std::any value)
{
    auto vars_list = getEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        var.get().assignValue(value);
    }
}

void EffectMaterial::assignVariableValues(const std::string& semantic, std::any value_array, unsigned value_count)
{
    auto vars_list = getEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        var.get().assignValues(value_array, value_count);
    }
}

void EffectMaterial::assignInPassVariableValue(const std::string& pass_name, const std::string& semantic, std::any value)
{
    auto var = getEffectVariableInPassBySemantic(pass_name, semantic);
    if (var) var.value().get().assignValue(std::move(value));
}

void EffectMaterial::assignInPassVariableValues(const std::string& pass_name, const std::string& semantic, std::any value_array, unsigned value_count)
{
    auto var = getEffectVariableInPassBySemantic(pass_name, semantic);
    if (var) var.value().get().assignValues(std::move(value_array), value_count);
}

void EffectMaterial::mappingAutoVariables()
{
    if (m_effectTechniques.empty()) return;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.getPassCount(); i++)
        {
            tech.getPassByIndex(i).mappingAutoVariables();
        }
    }
}

void EffectMaterial::selectTechnique()
{
    std::string combinedTechName = m_selectedRendererTechName + "/" + m_selectedVisualTechName;
    if ((m_currentTechnique != m_effectTechniques.end())
        && (m_currentTechnique->name() == combinedTechName)) return;
    if (m_effectTechniques.empty()) return;
    auto iter = std::find_if(m_effectTechniques.begin(), m_effectTechniques.end(),
        [=](auto& t) -> bool { return t.name() == combinedTechName; });
    if (iter != m_effectTechniques.end())
    {
        m_currentTechnique = iter;
        return;
    }
    m_currentTechnique = m_effectTechniques.begin(); // use first as default
}
