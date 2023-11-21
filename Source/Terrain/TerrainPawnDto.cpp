#include "TerrainPawnDto.h"
#include "TerrainPawn.h"
#include "SceneGraph/Pawn.h"

using namespace Enigma::Terrain;

TerrainPawnDto::TerrainPawnDto() : PawnDto()
{
    m_factoryDesc = Engine::FactoryDesc(TerrainPawn::TYPE_RTTI.getName());
}

TerrainPawnDto::TerrainPawnDto(const SceneGraph::PawnDto& dto) : PawnDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), TerrainPawn::TYPE_RTTI.getName()));
}

TerrainPawnDto TerrainPawnDto::fromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPawnDto pawn_dto(PawnDto::fromGenericDto(dto));
    return pawn_dto;
}

Enigma::Engine::GenericDto TerrainPawnDto::toGenericDto() const
{
    Engine::GenericDto dto = PawnDto::toGenericDto();
    return dto;
}

