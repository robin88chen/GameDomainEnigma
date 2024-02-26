/*********************************************************************
 * \file   StandardGeometryAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef STANDARD_GEOMETRY_ASSEMBLERS_H
#define STANDARD_GEOMETRY_ASSEMBLERS_H

#include <string>
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "GeometryDataDto.h"
#include "GraphicKernel/VertexDescription.h"
#include "GeometryPersistenceLevel.h"

namespace Enigma::Geometries
{
    class GeometryData;

    class SquareQuadAssembler
    {
    public:
        SquareQuadAssembler(const GeometryId& id);
        SquareQuadAssembler& xyQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadAssembler& xzQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadAssembler& normal();
        SquareQuadAssembler& textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        SquareQuadAssembler& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<GeometryData> constitute(PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        GeometryId m_id;
        TriangleListDto m_dto;
        MathLib::Vector3 m_normal;
        Graphics::VertexFormatCode m_format;
    };

    class CubeAssembler
    {
    public:
        CubeAssembler(const GeometryId& id);
        CubeAssembler& cube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent); // 8 vertices
        CubeAssembler& facedCube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent); // 24 vertices
        CubeAssembler& normal();
        CubeAssembler& facedNormal();
        CubeAssembler& textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        CubeAssembler& facedTextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);
        CubeAssembler& textureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back);
        CubeAssembler& facedTextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back);
        CubeAssembler& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<GeometryData> constitute(PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        GeometryId m_id;
        TriangleListDto m_dto;
        Graphics::VertexFormatCode m_format;
    };

    class SphereAssembler
    {
    public:
        SphereAssembler(const GeometryId& id);
        SphereAssembler& sphere(const MathLib::Vector3& center, float radius, int slices, int stacks);
        SphereAssembler& normal();
        SphereAssembler& textureCoord();
        SphereAssembler& sphereBound();
        SphereAssembler& boxBound();
        SphereAssembler& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<GeometryData> constitute(PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        GeometryId m_id;
        TriangleListDto m_dto;
        Graphics::VertexFormatCode m_format;
        MathLib::Vector3 m_center;
        float m_radius;
        std::vector<MathLib::Vector3> m_normals;  //< pre-calculated normals
        std::vector<MathLib::Vector2> m_tex_coords; //< pre-calculated texture coordinates
    };
}

#endif // STANDARD_GEOMETRY_ASSEMBLERS_H
