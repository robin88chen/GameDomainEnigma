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
            const Enigma::MathLib::Vector3& local_pos)
                : m_name(name), m_geometryDto(geometry_dto), m_layerTextures(layer_textures), m_localPos(local_pos) {}

        const std::string& GetName() const { return m_name; }
        const Enigma::Terrain::TerrainGeometryDto& GetGeometryDto() const { return m_geometryDto; }
        const std::array<std::string, TerrainEditService::TextureLayerNum>& GetLayerTextures() const { return m_layerTextures; }
        const Enigma::MathLib::Vector3& GetLocalPos() const { return m_localPos; }

    protected:
        std::string m_name;
        Enigma::Terrain::TerrainGeometryDto m_geometryDto;
        std::array<std::string, TerrainEditService::TextureLayerNum> m_layerTextures;
        Enigma::MathLib::Vector3 m_localPos;
    };
}

#endif // LEVEL_EDITOR_COMMANDS_H
