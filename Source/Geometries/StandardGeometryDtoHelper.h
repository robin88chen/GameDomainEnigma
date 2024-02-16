/*********************************************************************
 * \file   StandardGeometryDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef STANDARD_GEOMETRY_DTO_HELPER_H
#define STANDARD_GEOMETRY_DTO_HELPER_H

#include <string>
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "GeometryDataDto.h"
#include "GraphicKernel/VertexDescription.h"

namespace Enigma::Geometries
{
    class SquareQuadDtoHelper
    {
    public:
        SquareQuadDtoHelper(const GeometryId& id);
        SquareQuadDtoHelper& xyQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadDtoHelper& xzQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadDtoHelper& normal();
        SquareQuadDtoHelper& textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);

        Engine::GenericDto toGenericDto();

    protected:
        TriangleListDto m_dto;
        MathLib::Vector3 m_normal;
        Graphics::VertexFormatCode m_format;
    };

    class CubeDtoHelper
    {
    public:
        CubeDtoHelper(const GeometryId& id);
        CubeDtoHelper& cube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent); // 8 vertices
        CubeDtoHelper& facedCube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent); // 24 vertices
        CubeDtoHelper& normal();
        CubeDtoHelper& facedNormal();
        CubeDtoHelper& textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        CubeDtoHelper& facedTextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        CubeDtoHelper& textureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back);
        CubeDtoHelper& facedTextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back);

        Engine::GenericDto toGenericDto();

    protected:
        TriangleListDto m_dto;
        Graphics::VertexFormatCode m_format;
    };

    class SphereDtoHelper
    {
    public:
        SphereDtoHelper(const GeometryId& id);
        SphereDtoHelper& sphere(const MathLib::Vector3& center, float radius, int slices, int stacks);
        SphereDtoHelper& normal();
        SphereDtoHelper& textureCoord();
        SphereDtoHelper& sphereBound();
        SphereDtoHelper& boxBound();

        Engine::GenericDto toGenericDto();

    protected:
        TriangleListDto m_dto;
        Graphics::VertexFormatCode m_format;
        MathLib::Vector3 m_center;
        float m_radius;
        std::vector<MathLib::Vector3> m_normals;  //< pre-calculated normals
        std::vector<MathLib::Vector2> m_tex_coords; //< pre-calculated texture coordinates
    };
}

#endif // STANDARD_GEOMETRY_DTO_HELPER_H
