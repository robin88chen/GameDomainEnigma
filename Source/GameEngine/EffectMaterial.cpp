#include "EffectMaterial.h"
#include "EngineErrors.h"
#include "Frameworks/Rtti.h"
#include "EffectMaterialDto.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(Engine, EffectMaterial);

EffectMaterial::EffectMaterial(const std::string& name, const std::vector<EffectTechnique>& techniques)
    : m_factoryDesc(EffectMaterial::TYPE_RTTI.getName())
{
    m_factoryDesc.ClaimAsNative(name);
    m_name = name;
    m_effectTechniques = techniques;
    m_currentTechnique = m_effectTechniques.end();
    MappingAutoVariables();
}

EffectMaterial::EffectMaterial(const EffectMaterial& eff) : m_factoryDesc(eff.m_factoryDesc)
{
    m_name = eff.m_name;
    m_sourceMaterial = eff.m_sourceMaterial.lock();
    m_effectTechniques = eff.m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();
    MappingAutoVariables();
}

EffectMaterial::EffectMaterial(EffectMaterial&& eff) noexcept : m_factoryDesc(eff.factoryDesc())
{
    m_name = std::move(eff.m_name);
    m_sourceMaterial = std::move(eff.m_sourceMaterial.lock());
    m_effectTechniques = std::move(eff.m_effectTechniques);
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = std::move(eff.m_instancedAssignFuncList);
    m_selectedRendererTechName = std::move(eff.m_selectedRendererTechName);
    m_selectedVisualTechName = std::move(eff.m_selectedVisualTechName);
    MappingAutoVariables();
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
    m_name = eff.m_name;
    m_sourceMaterial = eff.m_sourceMaterial.lock();
    m_effectTechniques = eff.m_effectTechniques;
    m_currentTechnique = m_effectTechniques.end();
    MappingAutoVariables();

    return *this;
}

EffectMaterial& EffectMaterial::operator=(EffectMaterial&& eff) noexcept
{
    m_factoryDesc = std::move(eff.m_factoryDesc);
    m_name = std::move(eff.m_name);
    m_sourceMaterial = std::move(eff.m_sourceMaterial.lock());
    m_effectTechniques = std::move(eff.m_effectTechniques);
    m_currentTechnique = m_effectTechniques.end();
    m_instancedAssignFuncList = std::move(eff.m_instancedAssignFuncList);
    m_selectedRendererTechName = std::move(eff.m_selectedRendererTechName);
    m_selectedVisualTechName = std::move(eff.m_selectedVisualTechName);
    MappingAutoVariables();

    return *this;
}

GenericDto EffectMaterial::SerializeDto()
{
    EffectMaterialDto dto;
    dto.Name() = m_name;
    dto.factoryDesc() = m_factoryDesc;
    return dto.ToGenericDto();
}

void EffectMaterial::SetSource(const std::shared_ptr<EffectMaterialSource>& mat_source)
{
    m_sourceMaterial = mat_source;
}

void EffectMaterial::SelectRendererTechnique(const std::string& renderer_tech_name)
{
    m_selectedRendererTechName = renderer_tech_name;
    SelectTechnique();
}

void EffectMaterial::SelectVisualTechnique(const std::string& visual_tech_name)
{
    m_selectedVisualTechName = visual_tech_name;
    SelectTechnique();
}

unsigned int EffectMaterial::GetPassCount()
{
    if (m_currentTechnique == m_effectTechniques.end()) return 0;
    return m_currentTechnique->GetPassCount();
}

error EffectMaterial::ApplyFirstPass()
{
    if (m_currentTechnique == m_effectTechniques.end()) return ErrorCode::noSelectedTechnique;
    return m_currentTechnique->ApplyFirstPass();
}

error EffectMaterial::ApplyNextPass()
{
    if (m_currentTechnique == m_effectTechniques.end()) return ErrorCode::noSelectedTechnique;
    return m_currentTechnique->ApplyNextPass();
}

bool EffectMaterial::HasNextPass()
{
    if (m_currentTechnique == m_effectTechniques.end()) return false;
    return m_currentTechnique->HasNextPass();
}

void EffectMaterial::CommitInstancedEffectVariables()
{
    if (m_instancedAssignFuncList.size() > 0)
    {
        for (auto& func : m_instancedAssignFuncList)
        {
            func.second(func.first.get());
        }
    }
}

std::list<std::reference_wrapper<EffectVariable>> EffectMaterial::GetEffectVariablesByName(const std::string& name)
{
    std::list<std::reference_wrapper<EffectVariable>> result_list;
    result_list.clear();
    if (m_effectTechniques.empty()) return result_list;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.GetPassCount(); i++)
        {
            if (auto eff_var = tech.GetPassByIndex(i).GetVariableByName(name))
                result_list.emplace_back(eff_var.value());
        }
    }
    return result_list;
}

std::list<std::reference_wrapper<EffectVariable>> EffectMaterial::GetEffectVariablesBySemantic(const std::string& semantic)
{
    std::list<std::reference_wrapper<EffectVariable>> result_list;
    result_list.clear();
    if (m_effectTechniques.empty()) return result_list;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.GetPassCount(); i++)
        {
            if (auto eff_var = tech.GetPassByIndex(i).GetVariableBySemantic(semantic))
                result_list.emplace_back(eff_var.value());
        }
    }
    return result_list;
}

stdext::optional_ref<EffectVariable> EffectMaterial::GetEffectVariableInPassByName(const std::string& pass_name, const std::string& name)
{
    if (m_effectTechniques.empty()) return std::nullopt;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.GetPassCount(); i++)
        {
            if (tech.GetPassByIndex(i).getName() == pass_name)
                return tech.GetPassByIndex(i).GetVariableByName(name);
        }
    }
    return std::nullopt;
}

stdext::optional_ref<EffectVariable> EffectMaterial::GetEffectVariableInPassBySemantic(const std::string& pass_name, const std::string& semantic)
{
    if (m_effectTechniques.empty()) return std::nullopt;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.GetPassCount(); i++)
        {
            if (tech.GetPassByIndex(i).getName() == pass_name)
                return tech.GetPassByIndex(i).GetVariableBySemantic(semantic);
        }
    }
    return std::nullopt;
}

void EffectMaterial::SetVariableAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn)
{
    auto vars_list = GetEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        var.get().SetValueAssignFunction(fn);
    }
}

void EffectMaterial::SetInstancedAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn)
{
    auto vars_list = GetEffectVariablesBySemantic(semantic);
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

void EffectMaterial::AssignVariableValue(const std::string& semantic, std::any value)
{
    auto vars_list = GetEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        var.get().AssignValue(value);
    }
}

void EffectMaterial::AssignVariableValues(const std::string& semantic, std::any value_array, unsigned value_count)
{
    auto vars_list = GetEffectVariablesBySemantic(semantic);
    if (vars_list.empty()) return;
    for (auto& var : vars_list)
    {
        var.get().AssignValues(value_array, value_count);
    }
}

void EffectMaterial::AssignInPassVariableValue(const std::string& pass_name, const std::string& semantic, std::any value)
{
    auto var = GetEffectVariableInPassBySemantic(pass_name, semantic);
    if (var) var.value().get().AssignValue(std::move(value));
}

void EffectMaterial::AssignInPassVariableValues(const std::string& pass_name, const std::string& semantic, std::any value_array, unsigned value_count)
{
    auto var = GetEffectVariableInPassBySemantic(pass_name, semantic);
    if (var) var.value().get().AssignValues(std::move(value_array), value_count);
}

void EffectMaterial::MappingAutoVariables()
{
    if (m_effectTechniques.empty()) return;
    for (auto& tech : m_effectTechniques)
    {
        for (unsigned int i = 0; i < tech.GetPassCount(); i++)
        {
            tech.GetPassByIndex(i).MappingAutoVariables();
        }
    }
}

void EffectMaterial::SelectTechnique()
{
    std::string combinedTechName = m_selectedRendererTechName + "/" + m_selectedVisualTechName;
    if ((m_currentTechnique != m_effectTechniques.end())
        && (m_currentTechnique->getName() == combinedTechName)) return;
    if (m_effectTechniques.empty()) return;
    auto iter = std::find_if(m_effectTechniques.begin(), m_effectTechniques.end(),
        [=](auto& t) -> bool { return t.getName() == combinedTechName; });
    if (iter != m_effectTechniques.end())
    {
        m_currentTechnique = iter;
        return;
    }
    m_currentTechnique = m_effectTechniques.begin(); // use first as default
}
