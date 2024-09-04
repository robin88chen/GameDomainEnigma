/*********************************************************************
 * \file   SceneGraphAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_ASSEMBLERS_H
#define SCENE_GRAPH_ASSEMBLERS_H

#include "GameEngine/BoundingVolume.h"
#include "SceneGraphDtos.h"
#include "SceneGraph/Spatial.h"
#include "SpatialAssembler.h"
#include "PortalDtos.h"
#include <string>

namespace Enigma::SceneGraph
{
    class Pawn;
    class PortalManagementNode;
    class VisibilityManagedNode;
    class LazyNode;
    class PortalZoneNode;
    class Portal;

    /*class PawnAssembler
    {
    public:
        PawnAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        SpatialAssembler& spatial();

        PawnAssembler& factory(const Engine::FactoryDesc& factory);
        PawnAssembler& primitive(const Primitives::PrimitiveId& primitive_id);
        PawnAssembler& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto() const;
        PawnDto toPawnDto() const;
        std::shared_ptr<Pawn> constitute();

    private:
        SpatialId m_id;
        SpatialAssembler m_spatialAssembler;
        PawnDto m_dto;
    };*/

    /*class NodeAssembler
    {
    public:
        NodeAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        SpatialAssembler& spatial();

        NodeAssembler& factory(const Engine::FactoryDesc& factory);
        NodeAssembler& child(const SpatialId& child_id);
        NodeAssembler& child(const SpatialId& child_id, const Engine::GenericDto& child_dto);
        NodeAssembler& child(const SpatialId& child_id, const SpatialAssembler* child);
        NodeAssembler& child(const SpatialId& child_id, const NodeAssembler* child);
        NodeAssembler& asNative(const std::string& file_at_path);

        void consistChildrenLocationBounding();

        Engine::GenericDto toGenericDto() const;
        NodeDto toNodeDto() const;
        std::shared_ptr<Node> constitute();

    private:
        SpatialId m_id;
        SpatialAssembler m_spatialAssembler;
        NodeDto m_dto;
        std::vector<SpatialAssembler*> m_children;
        std::vector<NodeAssembler*> m_childNodes;
    };*/

    /*class LazyNodeAssembler
    {
    public:
        LazyNodeAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        NodeAssembler& node();

        LazyNodeAssembler& factory(const Engine::FactoryDesc& factory);
        LazyNodeAssembler& asDeferred(const std::string& file_name, const std::string& path_id);

        Engine::GenericDto toHydratedGenericDto() const;
        LazyNodeDto toHydratedDto() const;
        std::shared_ptr<LazyNode> constituteHydrated();
        Engine::GenericDto toDeHydratedGenericDto() const;
        LazyNodeDto toDeHydratedDto() const;
        std::shared_ptr<LazyNode> constituteDeHydrated();

    private:
        SpatialId m_id;
        NodeAssembler m_nodeAssembler;
        LazyNodeDto m_dto;
    };*/


    /*class VisibilityManagedNodeAssembler
    {
    public:
        VisibilityManagedNodeAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        LazyNodeAssembler& lazyNode();

        VisibilityManagedNodeAssembler& factory(const Engine::FactoryDesc& factory);
        VisibilityManagedNodeAssembler& asDeferred(const std::string& file_name, const std::string& path_id);

        Engine::GenericDto toHydratedGenericDto() const;
        VisibilityManagedNodeDto toHydratedDto() const;
        std::shared_ptr<VisibilityManagedNode> constituteHydrated();
        Engine::GenericDto toDeHydratedGenericDto() const;
        VisibilityManagedNodeDto toDeHydratedDto() const;
        std::shared_ptr<VisibilityManagedNode> constituteDeHydrated();

    private:
        SpatialId m_id;
        LazyNodeAssembler m_lazyNodeAssembler;
        VisibilityManagedNodeDto m_dto;
    };*/
}

#endif // SCENE_GRAPH_ASSEMBLERS_H
