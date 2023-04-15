#include "AvatarRecipes.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "GameEngine/EffectRequests.h"
#include "GameEngine/EffectResponses.h"
#include "Renderer/ModelPrimitive.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;

ReplaceAvatarMaterial::ReplaceAvatarMaterial(const std::string& old_material_name, const EffectMaterialDto& new_material_dto)
    : m_oldMaterialName(old_material_name), m_newMaterialDto(new_material_dto)
{
    m_onCompileEffectResponse = std::make_shared<ResponseSubscriber>([=](auto r) { this->OnCompileEffectResponse(r); });
    ResponseBus::Subscribe(typeid(CompileEffectMaterialResponse), m_onCompileEffectResponse);
}

ReplaceAvatarMaterial::~ReplaceAvatarMaterial()
{
    ResponseBus::Unsubscribe(typeid(CompileEffectMaterialResponse), m_onCompileEffectResponse);
    m_onCompileEffectResponse = nullptr;
    m_changeSpecifyMaterialMap.clear();
}

void ReplaceAvatarMaterial::Bake(const std::shared_ptr<Pawn>& pawn)
{
    if (!pawn) return;
    if ((m_oldMaterialName.empty()) || (m_newMaterialDto.Name().empty())) return;
    if (m_oldMaterialName == m_newMaterialDto.Name()) return;
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
                ReplaceMeshMaterial(mesh);
            }
        }
    }
    MeshPrimitivePtr mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(prim);
    if (mesh)
    {
        ReplaceMeshMaterial(mesh);
    }
    prim->SelectVisualTechnique(prim->GetSelectedVisualTechnique());
}

void ReplaceAvatarMaterial::ReplaceMeshMaterial(const MeshPrimitivePtr& mesh)
{
    if (!mesh) return;
    if ((m_oldMaterialName.empty()) || (m_newMaterialDto.Name().empty())) return;
    if (m_oldMaterialName == m_newMaterialDto.Name()) return;

    unsigned int total_mat_count = mesh->GetEffectMaterialCount();
    if (total_mat_count == 0) return;
    for (unsigned int i = 0; i < total_mat_count; i++)
    {
        EffectMaterialPtr eff = mesh->GetEffectMaterial(i);
        if (!eff) continue;
        if (eff->GetName() == m_oldMaterialName)
        {
            //todo: 先丟 request
            auto req = std::make_shared<RequestCompileEffectMaterial>(m_newMaterialDto);
            m_changeSpecifyMaterialMap[req->GetRuid()] = [mesh, i](const EffectMaterialPtr& eff_new)
            {
                mesh->ChangeEffectMaterialInSegment(i, eff_new);
                mesh->CreateRenderElements();
            };
            RequestBus::Post(req);
        }
    }
}

void ReplaceAvatarMaterial::OnCompileEffectResponse(const IResponsePtr& r)
{
    if (!r) return;
    auto res = std::dynamic_pointer_cast<CompileEffectMaterialResponse, IResponse>(r);
    if (!res) return;
    auto it = m_changeSpecifyMaterialMap.find(res->GetRequestRuid());
    if (it == m_changeSpecifyMaterialMap.end()) return;
    if (res->GetErrorCode())
    {
        return;
    }
    it->second(res->GetEffect());
}
