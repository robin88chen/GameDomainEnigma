/*********************************************************************
 * \file   TerrainEditService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_EDIT_SERVICE_H
#define TERRAIN_EDIT_SERVICE_H


#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "Terrain/TerrainPawn.h"
#include "Terrain/TerrainGeometry.h"
#include "Terrain/TerrainPrimitive.h"

namespace LevelEditor
{
    class TerrainEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TerrainEditService(Enigma::Frameworks::ServiceManager* srv_mngr);
        TerrainEditService(const TerrainEditService&) = delete;
        TerrainEditService& operator=(const TerrainEditService&) = delete;
        virtual ~TerrainEditService() override;
        TerrainEditService(TerrainEditService&&) = delete;
        TerrainEditService& operator=(TerrainEditService&&) = delete;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

    protected:
        void DoCreatingNewTerrain(const Enigma::Frameworks::ICommandPtr& c);
        void DoMovingUpTerrainVertex(const Enigma::Frameworks::ICommandPtr& c);
        void DoPaintingTerrainLayer(const Enigma::Frameworks::ICommandPtr& c);
        void DoCompletingEditOperation(const Enigma::Frameworks::ICommandPtr& c);
        void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
        void OnPickedSpatialChanged(const Enigma::Frameworks::IEventPtr& e);

        void MoveUpTerrainVertexByBrush(const Enigma::MathLib::Vector3& brush_pos, float brush_size, float height);
        void MoveUpTerrainVertex(const std::shared_ptr<Enigma::Terrain::TerrainGeometry>& terrain_geometry, const Enigma::MathLib::Vector3& picking_pos, float height);
        void CommitHeightMapUpdated(const std::shared_ptr<Enigma::Terrain::TerrainPrimitive>& terrain_primitive, const std::shared_ptr<Enigma::Terrain::TerrainGeometry>& terrain_geometry);

    public:
        static constexpr inline unsigned TextureLayerNum = 4;
        static std::array<std::string, TextureLayerNum> LayerSemantics;

    protected:
        std::weak_ptr<Enigma::Terrain::TerrainPawn> m_pickedTerrain;

        Enigma::Frameworks::CommandSubscriberPtr m_doCreatingNewTerrain;
        Enigma::Frameworks::CommandSubscriberPtr m_doMovingUpTerrainVertex;
        Enigma::Frameworks::CommandSubscriberPtr m_doPaintingTerrainLayer;
        Enigma::Frameworks::CommandSubscriberPtr m_doCompletingEditOperation;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onPickedSpatialChanged;

        bool m_isHeightMapDirty;
        unsigned m_dirtyVtxMinIndex;
        unsigned m_dirtyVtxMaxIndex;
    };
}

#endif // TERRAIN_EDIT_SERVICE_H
