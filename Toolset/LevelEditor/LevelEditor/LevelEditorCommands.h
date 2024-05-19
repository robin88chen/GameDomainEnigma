/*********************************************************************
 * \file   LevelEditorCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef LEVEL_EDITOR_COMMANDS_H
#define LEVEL_EDITOR_COMMANDS_H

#include "Frameworks/Command.h"
#include "SceneGraph/Spatial.h"
#include "Terrain/TerrainGeometryDto.h"
#include "GameEngine/GenericDto.h"
#include "TerrainEditService.h"
#include <vector>

namespace LevelEditor
{
    class OutputMessage : public Enigma::Frameworks::ICommand
    {
    public:
        OutputMessage(const std::string& msg) : m_msg(msg) {}

        const std::string& message() const { return m_msg; }

    private:
        std::string m_msg;
    };

    class RefreshSceneGraph : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshSceneGraph(std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>> flattened_spatial) : m_flattenedSpatial(
            std::move(flattened_spatial)) {}

        const std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>>& getFlattenedSpatial() const { return m_flattenedSpatial; }

    protected:
        std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>> m_flattenedSpatial;
    };

    class CreateNewTerrain : public Enigma::Frameworks::ICommand
    {
    public:
        CreateNewTerrain(const Enigma::Geometries::GeometryId& geometry_id, unsigned numRows, unsigned numCols, const Enigma::MathLib::Vector3& minVtxPos, const Enigma::MathLib::Vector3& maxVtxPos, const Enigma::MathLib::Vector2& minUV, const Enigma::MathLib::Vector2& maxUV, const std::array<Enigma::Engine::TextureId, TerrainEditService::TextureLayerNum> layer_textures, const Enigma::MathLib::Vector3& local_pos, const std::string& asset_path_id)
            : m_geometryId(geometry_id), m_numRows(numRows), m_numCols(numCols), m_minVtxPos(minVtxPos), m_maxVtxPos(maxVtxPos), m_minUV(minUV), m_maxUV(maxUV), m_layerTextures(layer_textures), m_localPos(local_pos), m_assetPathId(asset_path_id) {}

        const Enigma::Geometries::GeometryId& geometryId() const { return m_geometryId; }
        unsigned numRows() const { return m_numRows; }
        unsigned numCols() const { return m_numCols; }
        const Enigma::MathLib::Vector3& minVtxPos() const { return m_minVtxPos; }
        const Enigma::MathLib::Vector3& maxVtxPos() const { return m_maxVtxPos; }
        const Enigma::MathLib::Vector2& minUV() const { return m_minUV; }
        const Enigma::MathLib::Vector2& maxUV() const { return m_maxUV; }
        const std::array<Enigma::Engine::TextureId, TerrainEditService::TextureLayerNum>& getLayerTextures() const { return m_layerTextures; }
        const Enigma::MathLib::Vector3& getLocalPos() const { return m_localPos; }
        const std::string& getAssetPathId() const { return m_assetPathId; }

    protected:
        Enigma::Geometries::GeometryId m_geometryId;
        unsigned m_numRows;
        unsigned m_numCols;
        Enigma::MathLib::Vector3 m_minVtxPos;
        Enigma::MathLib::Vector3 m_maxVtxPos;
        Enigma::MathLib::Vector2 m_minUV;
        Enigma::MathLib::Vector2 m_maxUV;
        std::array<Enigma::Engine::TextureId, TerrainEditService::TextureLayerNum> m_layerTextures;
        Enigma::MathLib::Vector3 m_localPos;
        std::string m_assetPathId;
    };

    class CreateEnvironmentLight : public Enigma::Frameworks::ICommand
    {
    public:
        CreateEnvironmentLight(const std::string& world_name) : m_worldName(world_name) {}

        const std::string& getWorldName() const { return m_worldName; }

    protected:
        std::string m_worldName;
    };

    //--------------------------------------------------------------------------------
    class MoveUpTerrainVertex : public Enigma::Frameworks::ICommand
    {
    public:
        MoveUpTerrainVertex(const Enigma::MathLib::Vector3& brush_pos, float brush_size, float brush_height)
            : m_brushPos(brush_pos), m_brushSize(brush_size), m_brushHeight(brush_height) {}

        const Enigma::MathLib::Vector3& getBrushPos() const { return m_brushPos; }
        float getBrushSize() const { return m_brushSize; }
        float getBrushHeight() const { return m_brushHeight; }

    protected:
        Enigma::MathLib::Vector3 m_brushPos;
        float m_brushSize;
        float m_brushHeight;
    };
    class PaintTerrainTextureLayer : public Enigma::Frameworks::ICommand
    {
    public:
        PaintTerrainTextureLayer(const Enigma::MathLib::Vector3& brush_pos, float brush_size, float brush_density, unsigned texture_layer_index)
            : m_brushPos(brush_pos), m_brushSize(brush_size), m_brushDensity(brush_density), m_textureLayerIndex(texture_layer_index) {}

        const Enigma::MathLib::Vector3& getBrushPos() const { return m_brushPos; }
        float getBrushSize() const { return m_brushSize; }
        float getBrushDensity() const { return m_brushDensity; }
        unsigned getTextureLayerIndex() const { return m_textureLayerIndex; }

    protected:
        Enigma::MathLib::Vector3 m_brushPos;
        float m_brushSize;
        float m_brushDensity;
        unsigned m_textureLayerIndex;
    };
    class CompleteTerrainEditOperation : public Enigma::Frameworks::ICommand
    {
    public:
        CompleteTerrainEditOperation(const Enigma::MathLib::Vector3& brush_pos) : m_brushPos(brush_pos) {}

        const Enigma::MathLib::Vector3& getBrushPos() const { return m_brushPos; }

    protected:
        Enigma::MathLib::Vector3 m_brushPos;
    };
    class SaveTerrainSplatTexture : public Enigma::Frameworks::ICommand
    {
    public:
        SaveTerrainSplatTexture(const std::string& path_id) : m_pathId(path_id) {}

        const std::string& getPathId() const { return m_pathId; }
    protected:
        std::string m_pathId;
    };
}

#endif // LEVEL_EDITOR_COMMANDS_H
