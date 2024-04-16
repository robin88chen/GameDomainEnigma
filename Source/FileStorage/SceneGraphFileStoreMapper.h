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
        class SpatialFileMap
        {
        public:
            SpatialFileMap(const std::string& filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
            ~SpatialFileMap();
            SpatialFileMap(const SpatialFileMap&) = delete;
            SpatialFileMap(SpatialFileMap&&) = delete;
            SpatialFileMap& operator=(const SpatialFileMap&) = delete;
            SpatialFileMap& operator=(SpatialFileMap&&) = delete;

            std::error_code connect();
            std::error_code disconnect();

            bool has(const SceneGraph::SpatialId& id);
            std::optional<Engine::GenericDto> query(const SceneGraph::SpatialId& id);
            std::error_code remove(const SceneGraph::SpatialId& id);
            std::error_code put(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto);

            //todo: 這個直接開放出來不太好，但是目前沒有更好的方法，不過至少是const, 不會被破壞
            const auto& map() const { return m_map; };

        protected:
            void deserializeMapperFile(const std::string& content);
            Engine::GenericDto deserializeDataTransferObjects(const std::string& filename);

            std::error_code serializeMapperFile();
            std::string extractFilename(const SceneGraph::SpatialId& id, const Engine::FactoryDesc& factory_desc);

            std::error_code serializeDataTransferObjects(const std::string& filename, const Engine::GenericDto& dto);

        protected:
            std::shared_ptr<Gateways::IDtoGateway> m_gateway;
            std::string m_filename;
            std::unordered_map<SceneGraph::SpatialId, std::string, SceneGraph::SpatialId::hash> m_map;
            mutable std::recursive_mutex m_lock;
        };
    public:
        SceneGraphFileStoreMapper(const std::string& mapper_filename, const std::string& lazied_mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        ~SceneGraphFileStoreMapper() override;
        SceneGraphFileStoreMapper(const SceneGraphFileStoreMapper&) = delete;
        SceneGraphFileStoreMapper(SceneGraphFileStoreMapper&&) = delete;
        SceneGraphFileStoreMapper& operator=(const SceneGraphFileStoreMapper&) = delete;
        SceneGraphFileStoreMapper& operator=(SceneGraphFileStoreMapper&&) = delete;

        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasCamera(const SceneGraph::SpatialId& id) override;
        virtual std::optional<Engine::GenericDto> queryCamera(const SceneGraph::SpatialId& id) override;
        virtual std::error_code removeCamera(const SceneGraph::SpatialId& id) override;
        virtual std::error_code putCamera(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto) override;

        virtual bool hasSpatial(const SceneGraph::SpatialId& id) override;
        virtual std::optional<Engine::GenericDto> querySpatial(const SceneGraph::SpatialId& id) override;
        virtual std::error_code removeSpatial(const SceneGraph::SpatialId& id) override;
        virtual std::error_code putSpatial(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto) override;

        virtual bool hasLaziedContent(const SceneGraph::SpatialId& id) override;
        virtual std::optional<Engine::GenericDto> queryLaziedContent(const SceneGraph::SpatialId& id) override;
        virtual std::error_code removeLaziedContent(const SceneGraph::SpatialId& id) override;
        virtual std::error_code putLaziedContent(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto) override;

    protected:
        bool m_hasConnected;
        SpatialFileMap m_spatialMap;
        SpatialFileMap m_laziedMap;
    };
}

#endif // SCENE_GRAPH_FILE_STORE_MAPPER_H
