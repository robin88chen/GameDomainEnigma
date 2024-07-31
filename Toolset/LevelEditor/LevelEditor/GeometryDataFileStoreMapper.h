/*********************************************************************
 * \file   GeometryDataFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef EDITOR_GEOMETRY_DATA_FILE_STORE_MAPPER_H
#define EDITOR_GEOMETRY_DATA_FILE_STORE_MAPPER_H

#include "FileStorage/GeometryDataFileStoreMapper.h"
namespace LevelEditor
{
    class GeometryDataFileStoreMapper : public Enigma::FileStorage::GeometryDataFileStoreMapper
    {
    public:
        GeometryDataFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        GeometryDataFileStoreMapper(const GeometryDataFileStoreMapper&) = delete;
        GeometryDataFileStoreMapper& operator=(const GeometryDataFileStoreMapper&) = delete;
        virtual ~GeometryDataFileStoreMapper() override;
        GeometryDataFileStoreMapper(GeometryDataFileStoreMapper&&) = delete;
        GeometryDataFileStoreMapper& operator=(GeometryDataFileStoreMapper&&) = delete;

        bool isGeometryNameDuplicated(const std::string& geometry_name);
    };
}

#endif // EDITOR_GEOMETRY_DATA_FILE_STORE_MAPPER_H
