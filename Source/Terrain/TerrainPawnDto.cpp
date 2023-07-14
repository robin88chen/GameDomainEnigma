#include "TerrainPawnDto.h"
#include "TerrainPawn.h"
#include "SceneGraph/Pawn.h"

using namespace Enigma::Terrain;

TerrainPawnDto::TerrainPawnDto(const SceneGraph::PawnDto& dto) : PawnDto(dto)
{
}

TerrainPawnDto TerrainPawnDto::FromGenericDto(const Engine::GenericDto& dto)
{
    TerrainPawnDto pawn_dto(PawnDto::FromGenericDto(dto));
    return pawn_dto;
}

Enigma::Engine::GenericDto TerrainPawnDto::ToGenericDto() const
{
    Engine::GenericDto dto = PawnDto::ToGenericDto();
    return dto;
}

