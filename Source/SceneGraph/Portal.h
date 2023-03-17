/*********************************************************************
 * \file   Portal.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef PORTAL_H
#define PORTAL_H

#include "Spatial.h"
#include "MathLib/Vector3.h"
#include "MathLib/Plane3.h"
#include <memory>
#include "GameEngine/LinkageResolver.h"

#define PORTAL_VERTEX_COUNT     4

namespace Enigma::SceneGraph
{
    class PortalZoneNode;
    class PortalDto;

    class Portal : public Spatial
    {
        DECLARE_EN_RTTI;
    public:
        Portal(const std::string& name);
        Portal(const PortalDto& dto);
        Portal(const Portal&) = delete;
        Portal(Portal&&) = delete;
        Portal& operator=(const Portal&) = delete;
        Portal& operator=(Portal&&) = delete;
        virtual ~Portal() override;

        virtual Engine::GenericDto SerializeDto() override;
        void ResolveFactoryLinkage(const std::string& adjacent_node_name, Engine::FactoryLinkageResolver<Spatial>& resolver);

        void SetAdjacentZone(const std::shared_ptr<PortalZoneNode>& node);
        std::shared_ptr<PortalZoneNode> GetAdjacentZone() const { return m_adjacentPortalZone.lock(); };

        /// Portal Open & Close, Closed Portal 就像關起來的門
        bool IsOpen() const { return m_isOpen; };
        void Open() { m_isOpen = true; };
        void Close() { m_isOpen = false; };

        virtual bool CanVisited() override { return true; };

        virtual error OnCullingVisible(Culler* culler, bool noCull) override;
        virtual error CullVisibleSet(Culler* culler, bool noCull) override;

        virtual error _UpdateWorldData(const MathLib::Matrix4& parentWorld) override;

        virtual SceneTraveler::TravelResult VisitBy(SceneTraveler* traveler) override;

        auto GetPortalQuadWorldPosition() const { return m_vecPortalQuadWorldPos; }; // 實作寫在這裡可以用 auto return

        void UpdatePortalQuad();

    protected:
        std::weak_ptr<PortalZoneNode> m_adjacentPortalZone;
        bool m_isOpen;

        enum class ZoneLoadStatus
        {
            None = 0,
            OnLoad,
            Done,
        };
        ZoneLoadStatus m_zoneLoadStatus;

        std::array<MathLib::Vector3, PORTAL_VERTEX_COUNT> m_vecPortalQuadWorldPos;
        MathLib::Plane3 m_quadWorldPlane;
    };

    using PortalPtr = std::shared_ptr<Portal>;
}

#endif // PORTAL_H
