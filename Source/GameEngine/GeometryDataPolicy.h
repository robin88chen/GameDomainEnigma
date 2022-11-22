/*********************************************************************
 * \file   GeometryDataPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_DATA_POLICY_H
#define _GEOMETRY_DATA_POLICY_H

#include "GeometryDataDto.h"
#include "DtoDeserializer.h"
#include <string>
#include <optional>

namespace Enigma::Engine
{
    class GeometryDataPolicy
    {
    public:
        GeometryDataPolicy() = default;
        GeometryDataPolicy(const std::string& name, const GenericDto& dto) : m_name(name), m_dto(dto) {}
        GeometryDataPolicy(const std::string& name, const std::string& deserialize_param,
            const std::shared_ptr<IDtoDeserializer>& deserializer) : m_name(name), m_parameter(deserialize_param), m_deserializer(deserializer) {}

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }

        [[nodiscard]] const std::optional<GenericDto>& GetDto() const { return m_dto; }

        [[nodiscard]] const std::string& Parameter() const { return m_parameter; }
        [[nodiscard]] const std::shared_ptr<IDtoDeserializer>& GetDeserializer() const { return m_deserializer; }

    protected:
        std::string m_name;
        std::optional<GenericDto> m_dto;
        std::string m_parameter;
        std::shared_ptr<IDtoDeserializer> m_deserializer;
    };
}

#endif // _GEOMETRY_DATA_POLICY_H
