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

        const std::string& GetMessage() const { return m_msg; }

    private:
        std::string m_msg;
    };

    class RefreshSceneGraph : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshSceneGraph(std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>> flattened_spatial) : m_flattenedSpatial(
            std::move(flattened_spatial)) {}

        const std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>>& GetFlattenedSpatial() const { return m_flattenedSpatial; }

    protected:
        std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>> m_flattenedSpatial;
    };

    class CreateNewTerrain : public Enigma::Frameworks::ICommand
    {
    public:
        CreateNewTerrain(const std::string& name, const Enigma::Terrain::TerrainGeometryDto& geometry_dto,
            const std::array<std::string, TerrainEditService::TextureLayerNum> layer_textures,
            const Enigma::MathLib::Vector3& local_pos, const std::string& asset_path_id)
            : m_name(name), m_geometryDto(geometry_dto), m_layerTextures(layer_textures), m_localPos(local_pos), m_assetPathId(asset_path_id) {}

        const std::string& GetName() const { return m_name; }
        const Enigma::Terrain::TerrainGeometryDto& GetGeometryDto() const { return m_geometryDto; }
        const std::array<std::string, TerrainEditService::TextureLayerNum>& GetLayerTextures() const { return m_layerTextures; }
        const Enigma::MathLib::Vector3& GetLocalPos() const { return m_localPos; }
        const std::string& GetAssetPathId() const { return m_assetPathId; }

    protected:
        std::string m_name;
        Enigma::Terrain::TerrainGeometryDto m_geometryDto;
        std::array<std::string, TerrainEditService::TextureLayerNum> m_layerTextures;
        Enigma::MathLib::Vector3 m_localPos;
        std::string m_assetPathId;
    };

    class CreateEnvironmentLight : public Enigma::Frameworks::ICommand
    {
    public:
        CreateEnvironmentLight(const std::string& world_name) : m_worldName(world_name) {}

        const std::string& GetWorldName() const { return m_worldName; }

    protected:
        std::string m_worldName;
    };

    //--------------------------------------------------------------------------------
    class MoveUpTerrainVertex : public Enigma::Frameworks::ICommand
    {
    public:
        MoveUpTerrainVertex(const Enigma::MathLib::Vector3& brush_pos, float brush_size, float brush_height)
            : m_brushPos(brush_pos), m_brushSize(brush_size), m_brushHeight(brush_height) {}

        const Enigma::MathLib::Vector3& GetBrushPos() const { return m_brushPos; }
        float GetBrushSize() const { return m_brushSize; }
        float GetBrushHeight() const { return m_brushHeight; }

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

        const Enigma::MathLib::Vector3& GetBrushPos() const { return m_brushPos; }
        float GetBrushSize() const { return m_brushSize; }
        float GetBrushDensity() const { return m_brushDensity; }
        unsigned GetTextureLayerIndex() const { return m_textureLayerIndex; }

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

        const Enigma::MathLib::Vector3& GetBrushPos() const { return m_brushPos; }

    protected:
        Enigma::MathLib::Vector3 m_brushPos;
    };
    class SaveTerrainSplatTexture : public Enigma::Frameworks::ICommand
    {
    public:
        SaveTerrainSplatTexture(const std::string& path_id) : m_pathId(path_id) {}

        const std::string& GetPathId() const { return m_pathId; }
    protected:
        std::string m_pathId;
    };
}

#endif // LEVEL_EDITOR_COMMANDS_H
