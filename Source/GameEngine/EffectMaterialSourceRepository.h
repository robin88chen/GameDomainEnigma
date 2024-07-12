/*********************************************************************
 * \file   EffectMaterialSourceRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_MATERIAL_SOURCE_REPOSITORY_H
#define EFFECT_MATERIAL_SOURCE_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/QuerySubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "EffectMaterialId.h"
#include <unordered_map>
#include <mutex>

namespace Enigma::Engine
{
    class EffectMaterialSourceStoreMapper;
    class EffectCompilingQueue;
    class EffectMaterial;
    class EffectMaterialSource;

    class EffectMaterialSourceRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        EffectMaterialSourceRepository(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<EffectMaterialSourceStoreMapper>& store_mapper);
        EffectMaterialSourceRepository(const EffectMaterialSourceRepository& rhs) = delete;
        EffectMaterialSourceRepository(EffectMaterialSourceRepository&& rhs) = delete;
        EffectMaterialSourceRepository& operator=(const EffectMaterialSourceRepository& rhs) = delete;
        EffectMaterialSourceRepository& operator=(EffectMaterialSourceRepository&& rhs) = delete;
        ~EffectMaterialSourceRepository() override;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        bool hasEffectMaterial(const EffectMaterialId& id);
        std::shared_ptr<EffectMaterial> queryEffectMaterial(const EffectMaterialId& id);
        void releaseEffectMaterial(const EffectMaterialId& id);

    private:
        void queryEffectMaterial(const Frameworks::IQueryPtr& q);
        void releaseEffectMaterial(const Frameworks::ICommandPtr& c);

    private:
        std::shared_ptr<EffectMaterialSourceStoreMapper> m_storeMapper;
        EffectCompilingQueue* m_compilingQueue;

        typedef std::unordered_map<EffectMaterialId, std::shared_ptr<EffectMaterialSource>, EffectMaterialId::hash> SourceMaterialMap;
        SourceMaterialMap m_sourceMaterials;
        std::recursive_mutex m_sourceMapLock;

        Frameworks::QuerySubscriberPtr m_queryEffectMaterial;
        Frameworks::CommandSubscriberPtr m_releaseEffectMaterial;
    };
}

#endif // EFFECT_MATERIAL_SOURCE_REPOSITORY_H
