/*****************************************************************
 * \file   ViewerRenderablesFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef VIEWER_RENDERABLES_FILE_STORE_MAPPER_H
#define VIEWER_RENDERABLES_FILE_STORE_MAPPER_H

#include "FileStorage/PrimitiveFileStoreMapper.h"

namespace EnigmaViewer
{
    class ViewerRenderablesFileStoreMapper : public Enigma::FileStorage::PrimitiveFileStoreMapper
    {
    public:
        ViewerRenderablesFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        ~ViewerRenderablesFileStoreMapper() override;

        std::vector<std::string> modelNames() const;
        std::optional<Enigma::Primitives::PrimitiveId> modelId(const std::string& model_name) const;
    };
}

#endif // VIEWER_RENDERABLES_FILE_STORE_MAPPER_H
