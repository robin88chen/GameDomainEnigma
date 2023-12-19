/*****************************************************************//**
 * \file   EffectTechnique.h
 * \brief  Effect technique value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_TECHNIQUE_H
#define EFFECT_TECHNIQUE_H

#include "EffectPass.h"
#include <string>
#include <vector>

namespace Enigma::Engine
{
    using error = std::error_code;

    class EffectTechnique
    {
    public:
        EffectTechnique(const std::string& name, const std::vector<EffectPass>& passes);
        EffectTechnique(const EffectTechnique& source_tech);
        EffectTechnique(EffectTechnique&& source_tech) noexcept;

        virtual ~EffectTechnique();
        EffectTechnique& operator=(const EffectTechnique& source_tech);
        EffectTechnique& operator=(EffectTechnique&& source_tech) noexcept;

        const std::string& name() { return m_name; }

        void addPass(const EffectPass& pass);
        unsigned int getPassCount() { return static_cast<unsigned int>(m_effectPasses.size()); };
        EffectPass& getPassByIndex(unsigned int index);
        const EffectPass& getPassByIndex(unsigned int index) const;

        error applyFirstPass();
        error applyNextPass();
        bool hasNextPass();

    protected:
        using EffectPassArray = std::vector<EffectPass>;

    protected:
        std::string m_name;

        EffectPassArray m_effectPasses;
        EffectPassArray::iterator m_currentApplyPass;
    };
};

#endif // EFFECT_TECHNIQUE_H
