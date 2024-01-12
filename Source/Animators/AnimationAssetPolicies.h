/*********************************************************************
 * \file   AnimationAssetPolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_POLICIES_H
#define _ANIMATION_ASSET_POLICIES_H

#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/GenericDto.h"
#include <string>
#include <memory>
#include <optional>

namespace Enigma::Animators
{
    class AnimationAssetPolicy
    {
    public:
        AnimationAssetPolicy() = default;
        AnimationAssetPolicy(const std::string& name, const Engine::GenericDto& dto) : m_name(name), m_dto(dto) {}
        AnimationAssetPolicy(const std::string& name, const std::string& deserialize_param,
            const std::shared_ptr<Engine::IDtoDeserializer>& deserializer) : m_name(name), m_parameter(deserialize_param), m_deserializer(deserializer) {}

        [[nodiscard]] const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        [[nodiscard]] const std::string& RttiName() const { return m_rttiName; }
        std::string& RttiName() { return m_rttiName; }

        [[nodiscard]] const std::optional<Engine::GenericDto>& GetDto() const { return m_dto; }

        [[nodiscard]] const std::string& Parameter() const { return m_parameter; }
        [[nodiscard]] const std::shared_ptr<Engine::IDtoDeserializer>& GetDeserializer() const { return m_deserializer; }

    protected:
        std::string m_name;
        std::string m_rttiName;
        std::optional<Engine::GenericDto> m_dto;
        std::string m_parameter;
        std::shared_ptr<Engine::IDtoDeserializer> m_deserializer;
    };
}

#endif // _ANIMATION_ASSET_POLICIES_H
