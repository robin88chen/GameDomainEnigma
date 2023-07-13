#include "TerrainPrimitive.h"
#include "TerrainPrimitiveDto.h"

using namespace Enigma::Terrain;
using namespace Enigma::Renderer;

DEFINE_RTTI(Terrain, TerrainPrimitive, MeshPrimitive);

TerrainPrimitive::TerrainPrimitive(const std::string& name) : MeshPrimitive(name)
{

}

TerrainPrimitive::~TerrainPrimitive()
{

}

TerrainPrimitive::TerrainPrimitive(const TerrainPrimitive& t) : MeshPrimitive(t)
{

}

TerrainPrimitive::TerrainPrimitive(TerrainPrimitive&& t) noexcept : MeshPrimitive(std::move(t))
{

}

TerrainPrimitive& TerrainPrimitive::operator=(const TerrainPrimitive& t)
{
    return *this;
}

TerrainPrimitive& TerrainPrimitive::operator=(TerrainPrimitive&& t) noexcept
{
    return *this;
}

Enigma::Engine::GenericDto TerrainPrimitive::SerializeDto() const
{
    TerrainPrimitiveDto dto(SerializeMeshDto());
    return dto.ToGenericDto();
}

