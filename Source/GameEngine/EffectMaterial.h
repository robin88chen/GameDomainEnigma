/*********************************************************************
 * \file   EffectMaterial.h
 * \brief  Effect material, 先弄成 value object 試試
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_MATERIAL_H
#define EFFECT_MATERIAL_H

#include "EffectVariable.h"
#include "EffectTechnique.h"
#include "GraphicKernel/IShaderProgram.h"
#include <string>
#include <system_error>
#include <list>

namespace Enigma::Engine
{
    using error = std::error_code;
    class EffectMaterialSource;

    class EffectMaterial
    {
    public:
        EffectMaterial(const std::string& source_filename);
        EffectMaterial(const EffectMaterial&);
        EffectMaterial(EffectMaterial&&);
        virtual ~EffectMaterial();
        EffectMaterial& operator=(const EffectMaterial&);
        EffectMaterial& operator=(EffectMaterial&&);

        void Create(const std::string& source_filename);

        const std::string& GetSourceFileName();

        /** select renderer & visual technique */
        void SelectRendererTechnique(const std::string& renderer_tech_name);
        void SelectVisualTechnique(const std::string& visual_tech_name);

        unsigned int GetPassCount();
        error ApplyFirstPass();
        error ApplyNextPass();
        bool HasNextPass();

        void CommitEffectVariables();

        std::list<std::reference_wrapper<EffectVariable>> GetShaderVariablesByName(const std::string& name);
        std::list<std::reference_wrapper<EffectVariable>> GetShaderVariablesBySemantic(const std::string& semantic);

        void SetVariableAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn);
        /** 每個 material instance 已經是獨立的一份 var, 似乎沒有必要了，先留著就是 */
        void SetInstancedAssignFunc(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn);
        void MappingAutoVariables();

        const Graphics::IShaderProgramPtr GetFirstPassShaderProgram();

    protected:
        void SelectTechnique();

    protected:
        typedef std::vector<EffectTechnique> EffectTechniqueArray;
        typedef std::list<std::pair<std::reference_wrapper<EffectVariable>, EffectVariable::VariableValueAssignFunc>>
            InstancedAssignFuncList;

    protected:
        std::string m_sourceFileName;
        std::weak_ptr<EffectMaterialSource> m_sourceMaterial;

        EffectTechniqueArray m_effectTechniques;
        EffectTechniqueArray::iterator m_currentTechnique;
        InstancedAssignFuncList m_instancedAssignFuncList;
        std::string m_selectedRendererTechName;
        std::string m_selectedVisualTechName;
    };
};

#endif // EFFECT_MATERIAL_H
