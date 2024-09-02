/*********************************************************************
 * \file   SceneGraphDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_DTOS_H
#define SCENE_GRAPH_DTOS_H

#include "GameEngine/GenericDto.h"
#include "Primitives/PrimitiveId.h"
#include "SpatialId.h"
#include "SpatialDto.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace Enigma::SceneGraph
{
    /*class NodeDto : public SpatialDto
    {
    public:
        class ChildDto
        {
        public:
            ChildDto() = default;
            ChildDto(const Engine::GenericDto& dto_from);
            ChildDto(const SpatialId& id);
            ChildDto(const SpatialId& id, const Engine::GenericDto& child_dto);

            const SpatialId& id() const { return m_id; }
            const std::optional<Engine::GenericDto>& dto() const { return m_dto; }

            Engine::GenericDto toGenericDto() const;
        protected:
            SpatialId m_id;
            std::optional<Engine::GenericDto> m_dto;
        };
    public:
        NodeDto();
        NodeDto(const SpatialDto& spatial_dto);
        NodeDto(const Engine::GenericDto& dto);

        const std::vector<ChildDto>& children() const { return m_children; }
        void children(const std::vector<ChildDto>& children) { m_children = children; }
        void pushChild(const ChildDto& child) { m_children.push_back(child); }

        Engine::GenericDto toGenericDto() const;

    protected:
        std::vector<SpatialId> tokensToIds(const std::vector<std::string>& child_tokens);
        std::vector<std::string> idsToTokens(const std::vector<SpatialId> child_ids) const;

    protected:
        std::vector<ChildDto> m_children;
    };*/

    class PawnDto : public SpatialDto
    {
    public:
        PawnDto();
        PawnDto(const SpatialDto& spatial_dto);
        PawnDto(const Engine::GenericDto& dto);

        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& primitiveId() const { return m_primitiveId; }
        void primitiveId(const Primitives::PrimitiveId& primitive_id) { m_primitiveId = primitive_id; }

        Engine::GenericDto toGenericDto() const;

    protected:
        std::optional<Primitives::PrimitiveId> m_primitiveId;
    };

    class LazyNodeDto : public NodeDto
    {
    public:
        LazyNodeDto();
        LazyNodeDto(const NodeDto& node_dto);
        LazyNodeDto(const Engine::GenericDto& dto);

        Engine::GenericDto toGenericDto() const;
    };

    class VisibilityManagedNodeDto : public LazyNodeDto
    {
    public:
        VisibilityManagedNodeDto();
        VisibilityManagedNodeDto(const LazyNodeDto& lazy_node_dto);
        VisibilityManagedNodeDto(const Engine::GenericDto& dto);

        Engine::GenericDto toGenericDto() const;
    };
}

#endif // SCENE_GRAPH_DTOS_H
