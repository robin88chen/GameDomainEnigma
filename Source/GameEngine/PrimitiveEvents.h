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

namespace Enigma::Engine
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
        PrimitiveConstituted(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive) :
            m_id(id), m_primitive(primitive) {};
        const PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Primitive>& primitive() { return m_primitive; }
    private:
        PrimitiveId m_id;
        std::shared_ptr<Primitive> m_primitive;
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
