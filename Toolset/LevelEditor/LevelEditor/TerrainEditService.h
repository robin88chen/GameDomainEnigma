/*********************************************************************
 * \file   TerrainEditService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_EDIT_SERVICE_H
#define TERRAIN_EDIT_SERVICE_H

#include "FileStorage/TextureFileStoreMapper.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "Terrain/TerrainPawn.h"
#include "Terrain/TerrainGeometry.h"
#include "Terrain/TerrainPrimitive.h"
#include "MathLib/Rect.h"
#include "FileSystem/IFile.h"

namespace LevelEditor
{
    class CreateNewTerrain;

    class TerrainEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TerrainEditService(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<Enigma::FileStorage::TextureFileStoreMapper>& texture_file_store_mapper);
        TerrainEditService(const TerrainEditService&) = delete;
        TerrainEditService& operator=(const TerrainEditService&) = delete;
        virtual ~TerrainEditService() override;
        TerrainEditService(TerrainEditService&&) = delete;
        TerrainEditService& operator=(TerrainEditService&&) = delete;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

    protected:
        void assembleTerrainGeometry(const std::shared_ptr<CreateNewTerrain>& cmd);
        Enigma::Engine::TextureId assembleTerrainSplatTexture(const std::shared_ptr<CreateNewTerrain>& cmd);
        Enigma::Primitives::PrimitiveId assembleTerrainPrimitive(const std::shared_ptr<CreateNewTerrain>& cmd, const Enigma::Engine::TextureId& splat_texture_id);
        std::shared_ptr<Enigma::Terrain::TerrainPawn> assembleTerrainPawn(const std::shared_ptr<CreateNewTerrain>& cmd, const Enigma::Primitives::PrimitiveId& terrain_primitive_id);
        void createNewTerrain(const Enigma::Frameworks::ICommandPtr& c);
        void moveUpTerrainVertex(const Enigma::Frameworks::ICommandPtr& c);
        void paintTerrainLayer(const Enigma::Frameworks::ICommandPtr& c);
        void saveSplatTexture(const Enigma::Frameworks::ICommandPtr& c); // service 有 splat texture, 所以由 service 來存

        void onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
        void onTerrainPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
        void onPickedSpatialChanged(const Enigma::Frameworks::IEventPtr& e);

        void onSplatTextureSaved(const Enigma::Frameworks::IEventPtr& e);
        void onSaveSplatTextureFailed(const Enigma::Frameworks::IEventPtr& e);
        void onTextureImageRetrieved(const Enigma::Frameworks::IEventPtr& e);
        void onRetrieveTextureImageFailed(const Enigma::Frameworks::IEventPtr& e);

        void moveUpTerrainVertexByBrush(const Enigma::MathLib::Vector3& brush_pos, float brush_size, float height);
        void moveUpTerrainVertex(const std::shared_ptr<Enigma::Terrain::TerrainGeometry>& terrain_geometry, const Enigma::MathLib::Vector3& picking_pos, float height);
        void commitHeightMapUpdated(const std::shared_ptr<Enigma::Terrain::TerrainPrimitive>& terrain_primitive, const std::shared_ptr<Enigma::Terrain::TerrainGeometry>& terrain_geometry);

        void paintTerrainLayerByBrush(const Enigma::MathLib::Vector3& brush_pos, float brush_size, unsigned layer_idx, float density);
        void paintTerrainLayer(const Enigma::MathLib::Vector3& picking_pos, unsigned layer_idx, float density);
        void addLayerAlpha(unsigned texel_x, unsigned texel_y, unsigned layer_idx, int density);
        void commitAlphaTexelUpdated();
    public:
        static constexpr inline unsigned TextureLayerNum = 4;
        static std::array<std::string, TextureLayerNum> LayerSemantics;

    protected:
        std::weak_ptr<Enigma::FileStorage::TextureFileStoreMapper> m_textureFileStoreMapper;
        std::weak_ptr<Enigma::Terrain::TerrainPawn> m_pickedTerrain;
        std::unordered_map<Enigma::SceneGraph::SpatialId, std::weak_ptr<Enigma::Engine::Texture>, Enigma::SceneGraph::SpatialId::hash> m_splatTextures;
        std::weak_ptr<Enigma::Engine::Texture> m_pickedSplatTexture;

        Enigma::Frameworks::CommandSubscriberPtr m_createNewTerrain;
        Enigma::Frameworks::CommandSubscriberPtr m_moveUpTerrainVertex;
        Enigma::Frameworks::CommandSubscriberPtr m_paintTerrainLayer;
        Enigma::Frameworks::CommandSubscriberPtr m_saveSplatTexture;

        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainPrimitiveBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onPickedSpatialChanged;

        Enigma::Frameworks::EventSubscriberPtr m_onSplatTextureSaved;
        Enigma::Frameworks::EventSubscriberPtr m_onSaveSplatTextureFailed;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureImageRetrieved;
        Enigma::Frameworks::EventSubscriberPtr m_onRetrieveTextureImageFailed;

        std::string m_terrainPathId;

        bool m_isHeightMapDirty;
        unsigned m_dirtyVtxMinIndex;
        unsigned m_dirtyVtxMaxIndex;
        byte_buffer m_alphaTexels;
        Enigma::MathLib::Rect m_alphaRect;
        Enigma::Frameworks::Ruid m_retrieveTextureImageRuid;
        byte_buffer m_dirtyAlphaTexels;
        Enigma::MathLib::Rect m_dirtyAlphaRect;

        Enigma::FileSystem::IFilePtr m_savingSplatTextureFile;
    };
}

#endif // TERRAIN_EDIT_SERVICE_H
