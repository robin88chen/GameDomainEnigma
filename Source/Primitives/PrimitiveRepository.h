/*********************************************************************
 * \file   PrimitiveRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_REPOSITORY_H
#define PRIMITIVE_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/QuerySubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "PrimitiveId.h"
#include "PrimitiveFactoryDelegate.h"
#include "GameEngine/GenericDto.h"
#include <mutex>

namespace Enigma::Primitives
{
    class Primitive;
    class PrimitiveStoreMapper;
    class PrimitiveFactory;

    class PrimitiveRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        PrimitiveRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<PrimitiveStoreMapper>& store_mapper);
        PrimitiveRepository(const PrimitiveRepository&) = delete;
        PrimitiveRepository(PrimitiveRepository&&) = delete;
        PrimitiveRepository& operator=(const PrimitiveRepository&) = delete;
        PrimitiveRepository& operator=(PrimitiveRepository&&) = delete;
        ~PrimitiveRepository() override;

        /// On Init
        virtual Frameworks::ServiceResult onInit() override;
        /// On Term
        virtual Frameworks::ServiceResult onTerm() override;

        void registerPrimitiveFactory(const std::string& rtti, const PrimitiveCreator& creator);

        std::uint64_t nextSequenceNumber();
        bool hasPrimitive(const PrimitiveId& id);
        std::shared_ptr<Primitive> queryPrimitive(const PrimitiveId& id);
        void removePrimitive(const PrimitiveId& id);
        void putPrimitive(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive);

    protected:
        void queryPrimitive(const Frameworks::IQueryPtr& q);
        void queryPrimitiveNextSequenceNumber(const Frameworks::IQueryPtr& q);
        void requestPrimitiveCreation(const Frameworks::IQueryPtr& r);
        void requestPrimitiveConstitution(const Frameworks::IQueryPtr& r);
        void putPrimitive(const Frameworks::ICommandPtr& c);
        void removePrimitive(const Frameworks::ICommandPtr& c);

        std::shared_ptr<Primitive> findCachedPrimitive(const PrimitiveId& id);
        void dumpRetainedPrimitives();

    protected:
        std::shared_ptr<PrimitiveStoreMapper> m_storeMapper;
        PrimitiveFactory* m_factory;

        //! ADR: 在 repository 中，map 是存放已生成 asset 的 cache, 不擁有asset, 所以改用 weak_ptr 
        std::unordered_map<PrimitiveId, std::weak_ptr<Primitive>, PrimitiveId::hash> m_primitives;
        std::recursive_mutex m_primitiveLock;

        Frameworks::QuerySubscriberPtr m_queryPrimitive;
        Frameworks::QuerySubscriberPtr m_queryPrimitiveNextSequenceNumber;
        Frameworks::QuerySubscriberPtr m_requestPrimitiveCreation;
        Frameworks::QuerySubscriberPtr m_requestPrimitiveConstitution;

        Frameworks::CommandSubscriberPtr m_putPrimitive;
        Frameworks::CommandSubscriberPtr m_removePrimitive;
    };
}

#endif // PRIMITIVE_REPOSITORY_H
