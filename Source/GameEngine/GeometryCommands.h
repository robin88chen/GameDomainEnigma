/*********************************************************************
 * \file   GeometryCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_COMMANDS_H
#define _GEOMETRY_COMMANDS_H

#include "Frameworks/Command.h"
#include "GeometryDataPolicy.h"
#include "GeometryDataFactory.h"

namespace Enigma::Engine
{
    class GeometryData;

    class CreateGeometry : public Frameworks::ICommand
    {
    public:
        CreateGeometry(const GeometryId& id, const Frameworks::Rtti& rtti) : m_id(id), m_rtti(rtti.getName()) {}
        const GeometryId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }

    private:
        GeometryId m_id;
        std::string m_rtti;
    };
    class ConstituteGeometry : public Frameworks::ICommand
    {
    public:
        ConstituteGeometry(const GeometryId& id, const GenericDto& dto) : m_id(id), m_dto(dto) {}
        const GeometryId& id() { return m_id; }
        const GenericDto& dto() { return m_dto; }

    private:
        GeometryId m_id;
        GenericDto m_dto;
    };

    class RegisterGeometryFactory : public Frameworks::ICommand
    {
    public:
        RegisterGeometryFactory(const std::string& rtti, const GeometryCreator& creator, const GeometryConstitutor& constitutor)
            : m_rtti(rtti), m_creator(creator), m_constitutor(constitutor) {}

        const std::string& rttiName() const { return m_rtti; }
        const GeometryCreator& creator() { return m_creator; }
        const GeometryConstitutor& constitutor() { return m_constitutor; }

    private:
        std::string m_rtti;
        GeometryCreator m_creator;
        GeometryConstitutor m_constitutor;
    };
    class UnRegisterGeometryFactory : public Frameworks::ICommand
    {
    public:
        UnRegisterGeometryFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& rttiName() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
}

#endif // _GEOMETRY_COMMANDS_H
