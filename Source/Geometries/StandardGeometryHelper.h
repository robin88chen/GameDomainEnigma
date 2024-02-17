/*********************************************************************
 * \file   StandardGeometryHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef STANDARD_GEOMETRY_HELPER_H
#define STANDARD_GEOMETRY_HELPER_H

#include "GeometryId.h"
#include "StandardGeometryDtoHelper.h"
#include "GeometryData.h"
#include "GeometryPersistenceLevel.h"

namespace Enigma::Geometries
{
    class SquareQuadHelper
    {
    public:
        SquareQuadHelper(const GeometryId& id);
        SquareQuadHelper& xyQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadHelper& xzQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadHelper& normal();
        SquareQuadHelper& textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        SquareQuadHelper& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<GeometryData> constitute(PersistenceLevel persistence_level);
    protected:
        GeometryId m_id;
        SquareQuadDtoHelper m_dto;
    };

    class CubeHelper
    {
    public:
        CubeHelper(const GeometryId& id);
        CubeHelper& cube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent); // 8 vertices
        CubeHelper& facedCube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent); // 24 vertices
        CubeHelper& normal();
        CubeHelper& facedNormal();
        CubeHelper& textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        CubeHelper& facedTextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        CubeHelper& textureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back);
        CubeHelper& facedTextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back);
        CubeHelper& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<GeometryData> constitute(PersistenceLevel persistence_level);
    protected:
        GeometryId m_id;
        CubeDtoHelper m_dto;
    };

    class SphereHelper
    {
    public:
        SphereHelper(const GeometryId& id);
        SphereHelper& sphere(const MathLib::Vector3& center, float radius, int slices, int stacks);
        SphereHelper& normal();
        SphereHelper& textureCoord();
        SphereHelper& sphereBound();
        SphereHelper& boxBound();
        SphereHelper& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<GeometryData> constitute(PersistenceLevel persistence_level);
    protected:
        GeometryId m_id;
        SphereDtoHelper m_dto;
    };
}

#endif // STANDARD_GEOMETRY_HELPER_H
