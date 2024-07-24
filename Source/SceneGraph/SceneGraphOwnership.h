/*********************************************************************
 * \file   SceneGraphOwnership.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 *********************************************************************/
#ifndef SCENE_GRAPH_OWNERSHIP_H
#define SCENE_GRAPH_OWNERSHIP_H

#include <memory>
#include <optional>
#include <system_error>

namespace Enigma::SceneGraph
{
    class Spatial;
    class SpatialId;

    using error = std::error_code;

    class Ownership
    {
    public:
        static const std::optional<SpatialId>& getAssignedOwner(const std::shared_ptr<Spatial>& owned);
        static error attachOwnership(const SpatialId& owner, const std::shared_ptr<Spatial>& owned);

    protected:
        static error attachOwnershipOfPortalZone(const SpatialId& owner, const std::shared_ptr<Spatial>& owned);
        static error attachOwnershipOfOutRegion(const SpatialId& owner, const std::shared_ptr<Spatial>& owned);
        static error attachOwnershipOfSpatial(const SpatialId& owner, const std::shared_ptr<Spatial>& owned);
    };
}

#endif // SCENE_GRAPH_OWNERSHIP_H
