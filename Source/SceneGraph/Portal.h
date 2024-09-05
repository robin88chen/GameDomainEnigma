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

    class Portal : public Spatial
    {
        DECLARE_EN_RTTI;
    public:
        Portal(const SpatialId& id);
        Portal(const Portal&) = delete;
        Portal(Portal&&) = delete;
        Portal& operator=(const Portal&) = delete;
        Portal& operator=(Portal&&) = delete;
        virtual ~Portal() override;

        static std::shared_ptr<Portal> queryPortal(const SpatialId& id);

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler) override;

        void adjacentZone(const std::shared_ptr<PortalZoneNode>& zone);
        std::shared_ptr<PortalZoneNode> adjacentZone();

        /// Portal Open & Close, Closed Portal 就像關起來的門
        bool isOpen() const { return m_isOpen; };
        void open() { m_isOpen = true; };
        void close() { m_isOpen = false; };

        virtual bool canVisited() override { return true; };

        virtual error onCullingVisible(Culler* culler, bool noCull) override;
        virtual error cullVisibleSet(Culler* culler, bool noCull) override;

        virtual error _updateWorldData(const MathLib::Matrix4& parentWorld) override;

        virtual SceneTraveler::TravelResult visitBy(SceneTraveler* traveler) override;

        auto getPortalQuadWorldPosition() const { return m_vecPortalQuadWorldPos; }; // 實作寫在這裡可以用 auto return

        void updatePortalQuad();

    protected:
        SpatialId m_adjacentZoneId;
        std::shared_ptr<PortalZoneNode> m_adjacentPortalZone;
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
