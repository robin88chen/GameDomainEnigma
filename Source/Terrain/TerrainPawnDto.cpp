#include "TerrainPawnDto.h"
#include "TerrainPawn.h"

using namespace Enigma::Terrain;

TerrainPawnDto TerrainPawnDto::FromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPawnDto pawn_dto;
    pawn_dto.PawnDto::FromGenericDto(dto);
    return pawn_dto;
}

Enigma::Engine::GenericDto TerrainPawnDto::ToGenericDto() const
{
    Engine::GenericDto dto = PawnDto::ToGenericDto();
    dto.AddRtti(Engine::FactoryDesc(TerrainPawn::TYPE_RTTI.GetName()));
    return dto;
}

