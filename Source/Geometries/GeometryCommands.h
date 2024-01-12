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

namespace Enigma::Geometries
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
        ConstituteGeometry(const GeometryId& id, const Engine::GenericDto& dto) : m_id(id), m_dto(dto) {}
        const GeometryId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }

    private:
        GeometryId m_id;
        Engine::GenericDto m_dto;
    };

    class PutGeometry : public Frameworks::ICommand
    {
    public:
        PutGeometry(const GeometryId& id, const std::shared_ptr<GeometryData>& geometry) : m_id(id), m_geometry(geometry) {}

        const GeometryId& id() { return m_id; }
        const std::shared_ptr<GeometryData>& geometry() { return m_geometry; }

    protected:
        GeometryId m_id;
        std::shared_ptr<GeometryData> m_geometry;
    };
    class RemoveGeometry : public Frameworks::ICommand
    {
    public:
        RemoveGeometry(const GeometryId& id) : m_id(id) {}

        const GeometryId& id() { return m_id; }

    protected:
        GeometryId m_id;
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
