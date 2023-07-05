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

namespace Enigma::Engine
{
    class GeometryData;

    class BuildGeometryData : public Frameworks::ICommand
    {
    public:
        BuildGeometryData(const GeometryDataPolicy& policy) : m_policy(policy) {}
        const GeometryDataPolicy& GetPolicy() { return m_policy; }

    private:
        GeometryDataPolicy m_policy;
    };
    using GeometryDtoFactory = std::function<std::shared_ptr<GeometryData> (const Engine::GenericDto& dto)>;

    class RegisterGeometryDtoFactory : public Frameworks::ICommand
    {
    public:
        RegisterGeometryDtoFactory(const std::string& rtti, const GeometryDtoFactory& factory)
            : m_rtti(rtti), m_factory(factory) {}

        const std::string& GetRtti() const { return m_rtti; }
        const GeometryDtoFactory& GetFactory() { return m_factory; }

    private:
        std::string m_rtti;
        GeometryDtoFactory m_factory;
    };
    class UnRegisterGeometryDtoFactory : public Frameworks::ICommand
    {
    public:
        UnRegisterGeometryDtoFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& GetRtti() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
}

#endif // _GEOMETRY_COMMANDS_H
