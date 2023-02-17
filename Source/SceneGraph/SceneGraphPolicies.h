/*********************************************************************
 * \file   SceneGraphPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SCENE_GRAPH_POLICIES_H
#define _SCENE_GRAPH_POLICIES_H

namespace Enigma::SceneGraph
{
    class PawnPolicy
    {
    public:
        PawnPolicy() = default;
        PawnPolicy(const std::string& name, const Engine::GenericDto& dto) : m_name(name), m_dto(dto) {}
        PawnPolicy(const std::string& name, const std::string& deserialize_param,
            const std::shared_ptr<Engine::IDtoDeserializer>& deserializer) : m_name(name), m_parameter(deserialize_param), m_deserializer(deserializer) {}

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }

        [[nodiscard]] const std::optional<Engine::GenericDto>& GetDto() const { return m_dto; }

        [[nodiscard]] const std::string& Parameter() const { return m_parameter; }
        [[nodiscard]] const std::shared_ptr<Engine::IDtoDeserializer>& GetDeserializer() const { return m_deserializer; }

    protected:
        std::string m_name;
        std::optional<Engine::GenericDto> m_dto;
        std::string m_parameter;
        std::shared_ptr<Engine::IDtoDeserializer> m_deserializer;
    };
}

#endif // _SCENE_GRAPH_POLICIES_H
