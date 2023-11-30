/*********************************************************************
 * \file   SceneGraphFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_FILE_STORE_MAPPER_H
#define SCENE_GRAPH_FILE_STORE_MAPPER_H

#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/FactoryDesc.h"
#include "SceneGraph/SceneGraphStoreMapper.h"

namespace Enigma::FileStorage
{
    class SceneGraphFileStoreMapper : public SceneGraph::SceneGraphStoreMapper
    {
    public:
        SceneGraphFileStoreMapper(const std::string& mapper_filename);
        ~SceneGraphFileStoreMapper() override;
        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasCamera(const SceneGraph::SpatialId& id) override;
        virtual std::shared_ptr<SceneGraph::Camera> queryCamera(const SceneGraph::SpatialId& id) override;
        virtual std::error_code removeCamera(const SceneGraph::SpatialId& id) override;
        virtual std::error_code putCamera(const SceneGraph::SpatialId& id, const std::shared_ptr<SceneGraph::Camera>& camera) override;

    protected:
        std::string serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::string extractFilename(const SceneGraph::SpatialId& id, const Engine::FactoryDesc& factory_desc);

    protected:
        bool m_has_connected;
        std::string m_mapper_filename;
        std::unordered_map<SceneGraph::SpatialId, std::string, SceneGraph::SpatialId::hash> m_filename_map;
    };
}

#endif // SCENE_GRAPH_FILE_STORE_MAPPER_H
