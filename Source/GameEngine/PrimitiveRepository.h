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
#include "PrimitiveId.h"
#include <mutex>

namespace Enigma::Engine
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

        PrimitiveFactory* factory() { return m_factory; }

        std::uint64_t nextSequenceNumber();
        bool hasPrimitive(const PrimitiveId& id);
        std::shared_ptr<Primitive> queryPrimitive(const PrimitiveId& id);
        void removePrimitive(const PrimitiveId& id);
        void putPrimitive(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive);

    protected:
        void queryPrimitive(const Frameworks::IQueryPtr& q);

    protected:
        std::shared_ptr<PrimitiveStoreMapper> m_storeMapper;
        PrimitiveFactory* m_factory;

        std::unordered_map<PrimitiveId, std::shared_ptr<Primitive>, PrimitiveId::hash> m_primitives;
        std::recursive_mutex m_primitiveLock;

        Frameworks::QuerySubscriberPtr m_queryPrimitive;
    };
}

#endif // PRIMITIVE_REPOSITORY_H
