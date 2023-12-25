/*********************************************************************
 * \file   SceneGraphDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_DTOS_H
#define SCENE_GRAPH_DTOS_H

#include "MathLib/Matrix4.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/GenericDto.h"
#include "SpatialId.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace Enigma::SceneGraph
{
    class PawnPolicy;

    class SpatialDto
    {
    public:
        SpatialDto();

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        [[nodiscard]] bool isTopLevel() const { return m_isTopLevel; }
        bool& isTopLevel() { return m_isTopLevel; }
        const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        SpatialId& id() { return m_id; }
        [[nodiscard]] const SpatialId& id() const { return m_id; }
        const std::string& parentName() const { return m_parentName; }
        std::string& parentName() { return m_parentName; }
        [[nodiscard]] MathLib::Matrix4 localTransform() const { return m_localTransform; }
        MathLib::Matrix4& localTransform() { return m_localTransform; }
        [[nodiscard]] MathLib::Matrix4 worldTransform() const { return m_worldTransform; }
        MathLib::Matrix4& worldTransform() { return m_worldTransform; }
        [[nodiscard]] unsigned int graphDepth() const { return m_graphDepth; }
        unsigned int& graphDepth() { return m_graphDepth; }
        [[nodiscard]] Engine::GenericDto modelBound() const { return m_modelBound; }
        Engine::GenericDto& modelBound() { return m_modelBound; }
        [[nodiscard]] Engine::GenericDto worldBound() const { return m_worldBound; }
        Engine::GenericDto& worldBound() { return m_worldBound; }
        [[nodiscard]] unsigned int cullingMode() const { return m_cullingMode; }
        unsigned int& cullingMode() { return m_cullingMode; }
        [[nodiscard]] unsigned int spatialFlag() const { return m_spatialFlag; }
        unsigned int& spatialFlag() { return m_spatialFlag; }
        [[nodiscard]] unsigned int notifyFlag() const { return m_notifyFlag; }
        unsigned int& notifyFlag() { return m_notifyFlag; }

        static SpatialDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        bool m_isTopLevel;
        std::string m_name;
        SpatialId m_id;
        std::string m_parentName;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_worldTransform;
        unsigned int m_graphDepth;
        Engine::GenericDto m_modelBound;
        Engine::GenericDto m_worldBound;
        unsigned int m_cullingMode;
        unsigned int m_spatialFlag;
        unsigned int m_notifyFlag;
    };

    class NodeDto : public SpatialDto
    {
    public:
        NodeDto();
        NodeDto(const SpatialDto& spatial_dto);

        const std::vector<std::string>& childNames() const { return m_childNames; }
        std::vector<std::string>& childNames() { return m_childNames; }

        static NodeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        std::vector<std::string> m_childNames;
    };

    class LightDto : public SpatialDto
    {
    public:
        LightDto();
        LightDto(const SpatialDto& spatial_dto);

        [[nodiscard]] Engine::GenericDto lightInfo() const { return m_lightInfo; }
        Engine::GenericDto& lightInfo() { return m_lightInfo; }

        static LightDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::GenericDto m_lightInfo;
    };

    class PawnDto : public SpatialDto
    {
    public:
        PawnDto();
        PawnDto(const SpatialDto& spatial_dto);

        [[nodiscard]] const std::optional<Engine::GenericDto>& primitive() const { return m_primitive; }
        std::optional<Engine::GenericDto>& primitive() { return m_primitive; }
        //[[nodiscard]] const Engine::FactoryDesc& PrimitiveFactoryDesc() const { return m_primitiveFactory; }
        //Engine::FactoryDesc& PrimitiveFactoryDesc() { return m_primitiveFactory; }

        static PawnDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

        std::shared_ptr<PawnPolicy> convertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer);
    protected:
        std::optional<Engine::GenericDto> m_primitive;
        //todo : prefab support
        //Engine::FactoryDesc m_primitiveFactory;
    };

    class LazyNodeDto : public NodeDto
    {
    public:
        LazyNodeDto();
        LazyNodeDto(const NodeDto& node_dto);

        static LazyNodeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
    };

    class VisibilityManagedNodeDto : public LazyNodeDto
    {
    public:
        VisibilityManagedNodeDto();
        VisibilityManagedNodeDto(const LazyNodeDto& lazy_node_dto);

        static VisibilityManagedNodeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
    };
}

#endif // SCENE_GRAPH_DTOS_H
