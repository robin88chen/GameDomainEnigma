/*********************************************************************
 * \file   PrimitiveCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_COMMANDS_H
#define PRIMITIVE_COMMANDS_H

#include "Frameworks/Command.h"
#include "PrimitiveFactory.h"
#include "GameEngine/GenericDto.h"
#include <string>

namespace Enigma::Primitives
{
    class PutPrimitive : public Frameworks::ICommand
    {
    public:
        PutPrimitive(const PrimitiveId& id, const std::shared_ptr<Primitive>& primitive) : m_id(id), m_primitive(primitive) {}

        const PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Primitive>& primitive() { return m_primitive; }

    protected:
        PrimitiveId m_id;
        std::shared_ptr<Primitive> m_primitive;
    };
    class RemovePrimitive : public Frameworks::ICommand
    {
    public:
        RemovePrimitive(const PrimitiveId& id) : m_id(id) {}

        const PrimitiveId& id() { return m_id; }

    protected:
        PrimitiveId m_id;
    };

    class RegisterPrimitiveFactory : public Frameworks::ICommand
    {
    public:
        RegisterPrimitiveFactory(const std::string& rtti, const PrimitiveCreator& creator, const PrimitiveConstitutor& constitutor)
            : m_rtti(rtti), m_creator(creator), m_constitutor(constitutor) {}

        const std::string& rttiName() const { return m_rtti; }
        const PrimitiveCreator& creator() { return m_creator; }
        const PrimitiveConstitutor& constitutor() { return m_constitutor; }

    private:
        std::string m_rtti;
        PrimitiveCreator m_creator;
        PrimitiveConstitutor m_constitutor;
    };
    class UnregisterPrimitiveFactory : public Frameworks::ICommand
    {
    public:
        UnregisterPrimitiveFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& rttiName() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
}

#endif // PRIMITIVE_COMMANDS_H
