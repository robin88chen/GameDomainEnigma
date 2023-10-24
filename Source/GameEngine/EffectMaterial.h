/*********************************************************************
 * \file   EffectMaterial.h
 * \brief  Effect material, render element 裡會參考到, value object 不適合
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_MATERIAL_H
#define EFFECT_MATERIAL_H

#include "EffectVariable.h"
#include "EffectTechnique.h"
#include "GraphicKernel/IShaderProgram.h"
#include "Frameworks/Rtti.h"
#include "FactoryDesc.h"
#include <string>
#include <system_error>
#include <list>

namespace Enigma::Engine
{
    using error = std::error_code;
    class EffectMaterialSource;
    class GenericDto;

    class EffectMaterial
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        EffectMaterial(const std::string& name, const std::vector<EffectTechnique>& techniques);
        EffectMaterial(const EffectMaterial&);
        EffectMaterial(EffectMaterial&&) noexcept;
        virtual ~EffectMaterial();
        EffectMaterial& operator=(const EffectMaterial&);
        EffectMaterial& operator=(EffectMaterial&&) noexcept;

        GenericDto SerializeDto();

        const std::string& getName() { return m_name; };

        const FactoryDesc& TheFactoryDesc() const { return m_factoryDesc; }
        FactoryDesc& TheFactoryDesc() { return m_factoryDesc; }

        void SetSource(const std::shared_ptr<EffectMaterialSource>& mat_source);
        std::shared_ptr<EffectMaterialSource> GetEffectMaterialSource() { return m_sourceMaterial.lock(); };

        /** select renderer & visual technique */
        void SelectRendererTechnique(const std::string& renderer_tech_name);
        void SelectVisualTechnique(const std::string& visual_tech_name);

        unsigned int GetPassCount();
        error ApplyFirstPass();
        error ApplyNextPass();
        bool HasNextPass();

        void CommitInstancedEffectVariables();

        std::list<std::reference_wrapper<EffectVariable>> GetEffectVariablesByName(const std::string& name);
        std::list<std::reference_wrapper<EffectVariable>> GetEffectVariablesBySemantic(const std::string& semantic);
        stdext::optional_ref<EffectVariable> GetEffectVariableInPassByName(const std::string& pass_name, const std::string& name);
        stdext::optional_ref<EffectVariable> GetEffectVariableInPassBySemantic(const std::string& pass_name, const std::string& semantic);

        void SetVariableAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn);
        /** 每個 material instance 已經是獨立的一份 var, 似乎沒有必要了，先留著就是 */
        void SetInstancedAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn);

        void AssignVariableValue(const std::string& semantic, std::any value);
        void AssignVariableValues(const std::string& semantic, std::any value_array, unsigned int value_count);
        void AssignInPassVariableValue(const std::string& pass_name, const std::string& semantic, std::any value);
        void AssignInPassVariableValues(const std::string& pass_name, const std::string& semantic, std::any value_array, unsigned int value_count);

        void MappingAutoVariables();

    protected:
        void SelectTechnique();

    protected:
        typedef std::vector<EffectTechnique> EffectTechniqueArray;
        typedef std::list<std::pair<std::reference_wrapper<EffectVariable>, EffectVariable::VariableValueAssignFunc>>
            InstancedAssignFuncList;

    protected:
        FactoryDesc m_factoryDesc;

        std::string m_name;
        std::weak_ptr<EffectMaterialSource> m_sourceMaterial;

        EffectTechniqueArray m_effectTechniques;
        EffectTechniqueArray::iterator m_currentTechnique;
        InstancedAssignFuncList m_instancedAssignFuncList;
        std::string m_selectedRendererTechName;
        std::string m_selectedVisualTechName;
    };
    using EffectMaterialPtr = std::shared_ptr<EffectMaterial>;
};

#endif // EFFECT_MATERIAL_H
