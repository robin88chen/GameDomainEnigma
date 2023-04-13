#include "AvatarBaker.h"
#include "SceneGraph/Pawn.h"
#include "Renderer/ModelPrimitive.h"
#include "GameEngine/EffectRequests.h"
#include "Frameworks/RequestBus.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

void ReplaceMeshMaterial(const Enigma::Renderer::MeshPrimitivePtr& mesh,
                         const std::string& old_mat_name, const EffectMaterialDto& new_mat_dto);

void AvatarBaker::ReplaceAvatarMaterial(const std::shared_ptr<SceneGraph::Pawn>& pawn, const std::string& old_material_name, const EffectMaterialDto&
                                        new_material_dto)
{
    if (!pawn) return;
    if ((old_material_name.empty()) || (new_material_dto.Name().empty())) return;
    if (old_material_name == new_material_dto.Name()) return;
    PrimitivePtr prim = pawn->GetPrimitive();
    if (!prim) return;
    ModelPrimitivePtr model = std::dynamic_pointer_cast<ModelPrimitive, Primitive>(prim);
    if (model)
    {
        unsigned int total_mesh_prim = model->GetMeshPrimitiveCount();
        for (unsigned int i = 0; i < total_mesh_prim; i++)
        {
            MeshPrimitivePtr mesh = model->GetMeshPrimitive(i);
            if (mesh)
            {
                ReplaceMeshMaterial(mesh, old_material_name, new_material_dto);
            }
        }
    }
    MeshPrimitivePtr mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(prim);
    if (mesh)
    {
        ReplaceMeshMaterial(mesh, old_material_name, new_material_dto);
    }
    prim->SelectVisualTechnique(prim->GetSelectedVisualTechnique());
}

void ReplaceMeshMaterial(const MeshPrimitivePtr& mesh,
                         const std::string& old_mat_name, const EffectMaterialDto& new_mat_dto)
{
    if (!mesh) return;
    if ((old_mat_name.empty()) || (new_mat_dto.Name().empty())) return;
    if (old_mat_name == new_mat_dto.Name()) return;

    unsigned int total_mat_count = mesh->GetEffectMaterialCount();
    if (total_mat_count == 0) return;
    for (unsigned int i = 0; i < total_mat_count; i++)
    {
        EffectMaterialPtr eff = mesh->GetEffectMaterial(i);
        if (!eff) continue;
        if (eff->GetName() == old_mat_name)
        {
            //todo: 先丟 request
            RequestBus::Post(std::make_shared<RequestCompileEffectMaterial>(new_mat_dto));
            /*Enigma::EffectMaterialPtr eff_new;
            if (Enigma::GraphicMain::IsUseAsync())
            {
                eff_mgr->AsyncQueryEffectMaterial(new_mat_dto, &eff_new).wait();
                mesh->ChangeEffectMaterialInSegment(i, eff_new);
                mesh->CreateRenderElements(true);
            }
            else
            {
                eff_mgr->QueryEffectMaterial(new_mat_dto, &eff_new);
                mesh->ChangeEffectMaterialInSegment(i, eff_new);
                mesh->CreateRenderElements(false);
            }*/
        }
    }
}
