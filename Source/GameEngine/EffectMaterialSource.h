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
#include <list>

namespace Enigma::Engine
{
    class EffectMaterialSource : public std::enable_shared_from_this<EffectMaterialSource>
    {
    public:
        EffectMaterialSource(const EffectMaterialId& id);
        EffectMaterialSource(std::shared_ptr<EffectMaterial> material);
        EffectMaterialSource(const EffectMaterialSource&) = delete;
        EffectMaterialSource(EffectMaterialSource&&) = delete;
        ~EffectMaterialSource();
        EffectMaterialSource& operator=(const EffectMaterialSource&) = delete;
        EffectMaterialSource& operator=(EffectMaterialSource&&) = delete;

        const EffectMaterialId& id() const { return m_id; };

        void linkSourceSelf();
        const std::shared_ptr<EffectMaterial>& self() const { return m_sourceEffectMaterial; };

        std::shared_ptr<EffectMaterial> cloneEffectMaterial();
        std::shared_ptr<EffectMaterial> duplicateEffectMaterial();

        void contentDuplicatedEffects();

        static std::function<void(const std::shared_ptr<EffectMaterialSource>&)> onDuplicatedEmpty;

    private:
        void duplicatedEffectDeleter(EffectMaterial* effect);

    private:
        EffectMaterialId m_id;
        Frameworks::LazyStatus m_lazyStatus;
        std::shared_ptr<EffectMaterial> m_sourceEffectMaterial;
        std::list<std::shared_ptr<EffectMaterial>> m_duplicatedEffects;
        std::atomic_uint32_t m_duplicateCount;
        std::uint32_t m_duplicatedSerial;
    };
    using EffectMaterialSourcePtr = std::shared_ptr<EffectMaterialSource>;
}

#endif // EFFECT_MATERIAL_SOURCE_H
