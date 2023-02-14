#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/ServiceManager.h"
#include "MathLib/Matrix4.h"
#include "SceneGraph/Pawn.h"
#include "SkinMeshModelMaker.h"
#include "SceneGraph/SceneGraphDtos.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;

std::vector<Enigma::Engine::GenericDto> SceneGraphMaker::MakeSceneGraphDtos()
{
    NodeDto root_dto;
    root_dto.Name() = "scene_root";
    root_dto.LocalTransform() = Matrix4::IDENTITY;
    root_dto.IsTopLevel() = true;
    root_dto.ChildNames() = { "child1", "child2" };
    NodeDto child1_dto;
    child1_dto.Name() = "child1";
    child1_dto.LocalTransform() = Matrix4::IDENTITY;
    NodeDto child2_dto;
    child2_dto.Name() = "child2";
    child2_dto.LocalTransform() = Matrix4::IDENTITY;
    child2_dto.ChildNames() = { "pawn" };
    PawnDto pawn_dto;
    pawn_dto.Name() = "pawn";
    pawn_dto.LocalTransform() = Matrix4::IDENTITY;
    pawn_dto.ThePrimitive() = SkinMeshModelMaker::MakeModelPrimitiveDto("test_model", "test_geometry").ToGenericDto();

    return { root_dto.ToGenericDto(), child1_dto.ToGenericDto(), child2_dto.ToGenericDto(), pawn_dto.ToGenericDto() };
}
