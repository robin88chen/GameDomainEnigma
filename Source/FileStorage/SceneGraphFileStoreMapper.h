/*********************************************************************
 * \file   SceneGraphFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_FILE_STORE_MAPPER_H
#define SCENE_GRAPH_FILE_STORE_MAPPER_H

#include "GameEngine/FactoryDesc.h"
#include "Gateways/DtoGateway.h"
#include "SceneGraph/SceneGraphStoreMapper.h"
#include "SceneGraph/Camera.h"
#include <mutex>

namespace Enigma::FileStorage
{
    class SceneGraphFileStoreMapper : public SceneGraph::SceneGraphStoreMapper
    {
    public:
        SceneGraphFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        ~SceneGraphFileStoreMapper() override;
        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasCamera(const SceneGraph::SpatialId& id) override;
        virtual Engine::GenericDtoCollection queryCamera(const SceneGraph::SpatialId& id) override;
        virtual std::error_code removeCamera(const SceneGraph::SpatialId& id) override;
        virtual std::error_code putCamera(const SceneGraph::SpatialId& id, const Engine::GenericDtoCollection& dtos) override;

    protected:
        std::error_code serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::string extractFilename(const SceneGraph::SpatialId& id, const Engine::FactoryDesc& factory_desc);

        std::error_code serializeDataTransferObjects(const std::string& filename, const Engine::GenericDtoCollection& dtos);
        Engine::GenericDtoCollection deserializeDataTransferObjects(const std::string& filename);

    protected:
        bool m_has_connected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<SceneGraph::SpatialId, std::string, SceneGraph::SpatialId::hash> m_filename_map;
        std::recursive_mutex m_fileMapLock;
    };
}

#endif // SCENE_GRAPH_FILE_STORE_MAPPER_H
