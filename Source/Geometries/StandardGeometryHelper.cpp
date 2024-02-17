#include "StandardGeometryHelper.h"
#include "GeometryDataQueries.h"

using namespace Enigma::Geometries;

SquareQuadHelper::SquareQuadHelper(const GeometryId& id) : m_id(id), m_dto(id)
{
}

SquareQuadHelper& SquareQuadHelper::xyQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top)
{
    m_dto.xyQuad(left_bottom, right_top);
    return *this;
}

SquareQuadHelper& SquareQuadHelper::xzQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top)
{
    m_dto.xzQuad(left_bottom, right_top);
    return *this;
}

SquareQuadHelper& SquareQuadHelper::normal()
{
    m_dto.normal();
    return *this;
}

SquareQuadHelper& SquareQuadHelper::textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    m_dto.textureCoord(left_bottom, right_top);
    return *this;
}

SquareQuadHelper& SquareQuadHelper::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_dto.asAsset(name, filename, path_id);
    return *this;
}

std::shared_ptr<GeometryData> SquareQuadHelper::constitute(PersistenceLevel persistence_level)
{
    return std::make_shared<RequestGeometryConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch();
}

CubeHelper::CubeHelper(const GeometryId& id) : m_id(id), m_dto(id)
{
}

CubeHelper& CubeHelper::cube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent)
{
    m_dto.cube(center, axis_extent);
    return *this;
}

CubeHelper& CubeHelper::facedCube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent)
{
    m_dto.facedCube(center, axis_extent);
    return *this;
}

CubeHelper& CubeHelper::normal()
{
    m_dto.normal();
    return *this;
}

CubeHelper& CubeHelper::facedNormal()
{
    m_dto.facedNormal();
    return *this;
}

CubeHelper& CubeHelper::textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    m_dto.textureCoord(left_bottom, right_top);
    return *this;
}

CubeHelper& CubeHelper::textureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back)
{
    m_dto.textureCoord(left_bottom_front, right_top_back);
    return *this;
}

CubeHelper& CubeHelper::facedTextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    m_dto.facedTextureCoord(left_bottom, right_top);
    return *this;
}

CubeHelper& CubeHelper::facedTextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back)
{
    m_dto.facedTextureCoord(left_bottom_front, right_top_back);
    return *this;
}

CubeHelper& CubeHelper::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_dto.asAsset(name, filename, path_id);
    return *this;
}

std::shared_ptr<GeometryData> CubeHelper::constitute(PersistenceLevel persistence_level)
{
    return std::make_shared<RequestGeometryConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch();
}

SphereHelper::SphereHelper(const GeometryId& id) : m_id(id), m_dto(id)
{
}

SphereHelper& SphereHelper::sphere(const MathLib::Vector3& center, float radius, int slices, int stacks)
{
    m_dto.sphere(center, radius, slices, stacks);
    return *this;
}

SphereHelper& SphereHelper::normal()
{
    m_dto.normal();
    return *this;
}

SphereHelper& SphereHelper::textureCoord()
{
    m_dto.textureCoord();
    return *this;
}

SphereHelper& SphereHelper::sphereBound()
{
    m_dto.sphereBound();
    return *this;
}

SphereHelper& SphereHelper::boxBound()
{
    m_dto.boxBound();
    return *this;
}

SphereHelper& SphereHelper::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_dto.asAsset(name, filename, path_id);
    return *this;
}

std::shared_ptr<GeometryData> SphereHelper::constitute(PersistenceLevel persistence_level)
{
    return std::make_shared<RequestGeometryConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch();
}
