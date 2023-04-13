/*********************************************************************
 * \file   EffectMaterialSource.h
 * \brief  effect material source entity
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_MATERIAL_SOURCE_H
#define EFFECT_MATERIAL_SOURCE_H

#include "EffectMaterial.h"
#include <memory>
#include <functional>
#include <atomic>

namespace Enigma::Engine
{
    class EffectMaterialSource : public std::enable_shared_from_this<EffectMaterialSource>
    {
    public:
        EffectMaterialSource();
        EffectMaterialSource(std::shared_ptr<EffectMaterial> material);
        EffectMaterialSource(const EffectMaterialSource&) = delete;
        EffectMaterialSource(EffectMaterialSource&&) = delete;
        ~EffectMaterialSource();
        EffectMaterialSource& operator=(const EffectMaterialSource&) = delete;
        EffectMaterialSource& operator=(EffectMaterialSource&&) = delete;

        const std::string& GetName() const;

        void LinkSource();

        EffectMaterialPtr CloneEffectMaterial();

        static std::function<void(const std::shared_ptr<EffectMaterialSource>&)> OnDuplicatedEmpty;

    private:
        void DuplicatedEffectDeleter(EffectMaterial* effect);

    private:
        std::shared_ptr<EffectMaterial> m_effectMaterial;
        std::atomic_uint32_t m_duplicateCount;
    };
    using EffectMaterialSourcePtr = std::shared_ptr<EffectMaterialSource>;
}

#endif // EFFECT_MATERIAL_SOURCE_H
