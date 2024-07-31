/*********************************************************************
 * \file   PrimitiveEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_EVENTS_H
#define PRIMITIVE_EVENTS_H

#include "PrimitiveId.h"
#include "Frameworks/Event.h"
#include <system_error>

namespace Enigma::Primitives
{
    class PrimitiveCreated : public Frameworks::IEvent
    {
    public:
        PrimitiveCreated(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive) :
            m_id(id), m_primitive(primitive) {};
        const PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Primitive>& primitive() { return m_primitive; }
    private:
        PrimitiveId m_id;
        std::shared_ptr<Primitive> m_primitive;
    };
    class CreatePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        CreatePrimitiveFailed(const PrimitiveId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        PrimitiveId m_id;
        std::error_code m_error;
    };
    class PrimitiveConstituted : public Frameworks::IEvent
    {
    public:
        PrimitiveConstituted(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive, bool is_persisted) :
            m_id(id), m_primitive(primitive), m_is_persisted(is_persisted) {};
        const PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Primitive>& primitive() { return m_primitive; }
        bool isPersisted() const { return m_is_persisted; }
    private:
        PrimitiveId m_id;
        std::shared_ptr<Primitive> m_primitive;
        bool m_is_persisted;
    };
    class ConstitutePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        ConstitutePrimitiveFailed(const PrimitiveId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        PrimitiveId m_id;
        std::error_code m_error;
    };
    class PrimitiveRemoved : public Frameworks::IEvent
    {
    public:
        PrimitiveRemoved(const PrimitiveId& id) : m_id(id) {}

        const PrimitiveId& id() { return m_id; }

    private:
        PrimitiveId m_id;
    };
    class RemovePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        RemovePrimitiveFailed(const PrimitiveId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        PrimitiveId m_id;
        std::error_code m_error;
    };
    class PrimitivePut : public Frameworks::IEvent
    {
    public:
        PrimitivePut(const PrimitiveId& id) : m_id(id) {}

        const PrimitiveId& id() { return m_id; }

    private:
        PrimitiveId m_id;
    };
    class PutPrimitiveFailed : public Frameworks::IEvent
    {
    public:
        PutPrimitiveFailed(const PrimitiveId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        PrimitiveId m_id;
        std::error_code m_error;
    };
}

#endif // PRIMITIVE_EVENTS_H
