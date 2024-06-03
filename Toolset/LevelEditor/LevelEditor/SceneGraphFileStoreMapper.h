/*****************************************************************
 * \file   SceneGraphFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef EDITOR_SCENE_GRAPH_FILE_STORE_MAPPER_H
#define EDITOR_SCENE_GRAPH_FILE_STORE_MAPPER_H

#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "Frameworks/QuerySubscriber.h"

namespace LevelEditor
{
    class SceneGraphFileStoreMapper : public Enigma::FileStorage::SceneGraphFileStoreMapper
    {
    public:
        SceneGraphFileStoreMapper(const std::string& mapper_filename, const std::string& lazied_mapper_filename, const std::string& lazied_prefix, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        ~SceneGraphFileStoreMapper() override;

        void subscribeHandlers();
        void unsubscribeHandlers();

        std::optional<Enigma::SceneGraph::SpatialId> spatialId(const std::string& spatial_name) const;

    protected:
        bool isSpatialNameDuplicated(const std::string& spatial_name) const;

        void isSpatialNameDuplicated(const Enigma::Frameworks::IQueryPtr& q);

    protected:
        Enigma::Frameworks::QuerySubscriberPtr m_isSpatialNameDuplicated;
    };
}

#endif // EDITOR_SCENE_GRAPH_FILE_STORE_MAPPER_H
