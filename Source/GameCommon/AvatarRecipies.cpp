#include "AvatarRecipes.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "GameEngine/EffectRequests.h"
#include "GameEngine/EffectResponses.h"
#include "GameEngine/TextureRequests.h"
#include "GameEngine/TextureResponses.h"
#include "Renderer/ModelPrimitive.h"
#include "AvatarRecipeDto.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(GameCommon, AvatarRecipe);
DEFINE_RTTI(GameCommon, ReplaceAvatarMaterial, AvatarRecipe);
DEFINE_RTTI(GameCommon, ChangeAvatarTexture, AvatarRecipe);

AvatarRecipe::AvatarRecipe() : m_factoryDesc(AvatarRecipe::TYPE_RTTI.GetName())
{
}

AvatarRecipe::AvatarRecipe(const Engine::GenericDto& o) : m_factoryDesc(o.GetRtti())
{
}

std::shared_ptr<AvatarRecipe> AvatarRecipe::CreateFromGenericDto(const Engine::GenericDto& dto)
{
    std::string type = dto.GetRtti().GetRttiName();
    if (type == ReplaceAvatarMaterial::TYPE_RTTI.GetName())
    {
        return std::make_shared<ReplaceAvatarMaterial>(dto);
    }
    else if (type == ChangeAvatarTexture::TYPE_RTTI.GetName())
    {
        return std::make_shared<ChangeAvatarTexture>(dto);
    }
    return nullptr;
}


ReplaceAvatarMaterial::ReplaceAvatarMaterial(const std::string& old_material_name, const EffectMaterialDto& new_material_dto)
    : m_oldMaterialName(old_material_name), m_newMaterialDto(new_material_dto)
{
    m_factoryDesc = FactoryDesc(ReplaceAvatarMaterial::TYPE_RTTI.GetName());
    m_onCompileEffectResponse = std::make_shared<ResponseSubscriber>([=](auto r) { this->OnCompileEffectResponse(r); });
    ResponseBus::Subscribe(typeid(CompileEffectMaterialResponse), m_onCompileEffectResponse);
}

ReplaceAvatarMaterial::ReplaceAvatarMaterial(const Engine::GenericDto& o) : AvatarRecipe(o)
{
    AvatarRecipeReplaceMaterialDto dto = AvatarRecipeReplaceMaterialDto::FromGenericDto(o);
    m_oldMaterialName = dto.OldMaterialName();
    m_newMaterialDto = dto.NewMaterialDto();
    m_onCompileEffectResponse = std::make_shared<ResponseSubscriber>([=](auto r) { this->OnCompileEffectResponse(r); });
    ResponseBus::Subscribe(typeid(CompileEffectMaterialResponse), m_onCompileEffectResponse);
}

ReplaceAvatarMaterial::~ReplaceAvatarMaterial()
{
    ResponseBus::Unsubscribe(typeid(CompileEffectMaterialResponse), m_onCompileEffectResponse);
    m_onCompileEffectResponse = nullptr;
    m_changeSpecifyMaterialMap.clear();
}

GenericDto ReplaceAvatarMaterial::SerializeDto() const
{
    AvatarRecipeReplaceMaterialDto dto;
    dto.TheFactoryDesc() = m_factoryDesc;
    dto.OldMaterialName() = m_oldMaterialName;
    dto.NewMaterialDto() = m_newMaterialDto;
    return dto.ToGenericDto();
}

void ReplaceAvatarMaterial::Bake(const std::shared_ptr<Pawn>& pawn)
{
    if (!pawn) return;
    if ((m_oldMaterialName.empty()) || (m_newMaterialDto.Name().empty())) return;
    if (m_oldMaterialName == m_newMaterialDto.Name()) return;
    PrimitivePtr prim = pawn->GetPrimitive();
    if (!prim) return;
    m_primitive = prim;
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
            m_changeSpecifyMaterialMap[req->GetRuid()] = [mesh, i, this](const EffectMaterialPtr& eff_new)
            {
                mesh->ChangeEffectMaterialInSegment(i, eff_new);
                mesh->CreateRenderElements();
                if (!m_primitive.expired())
                {
                    m_primitive.lock()->SelectVisualTechnique(m_primitive.lock()->GetSelectedVisualTechnique());
                }
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
    if (res->GetErrorCode()) return;
    it->second(res->GetEffect());
}

ChangeAvatarTexture::ChangeAvatarTexture(const std::string& mesh_name, const TextureMappingDto& texture_dto)
    : m_meshName(mesh_name), m_textureDto(texture_dto), m_requsetRuid()
{
    m_factoryDesc = FactoryDesc(ChangeAvatarTexture::TYPE_RTTI.GetName());
    m_onLoadTextureResponse = std::make_shared<ResponseSubscriber>([=](auto r) { this->OnLoadTextureResponse(r); });
    ResponseBus::Subscribe(typeid(LoadTextureResponse), m_onLoadTextureResponse);
}

ChangeAvatarTexture::ChangeAvatarTexture(const Engine::GenericDto& o) : AvatarRecipe(o), m_requsetRuid()
{
    AvatarRecipeChangeTextureDto dto = AvatarRecipeChangeTextureDto::FromGenericDto(o);
    m_meshName = dto.MeshName();
    m_textureDto = dto.TextureDto();
    m_onLoadTextureResponse = std::make_shared<ResponseSubscriber>([=](auto r) { this->OnLoadTextureResponse(r); });
    ResponseBus::Subscribe(typeid(LoadTextureResponse), m_onLoadTextureResponse);
}

ChangeAvatarTexture::~ChangeAvatarTexture()
{
    ResponseBus::Unsubscribe(typeid(LoadTextureResponse), m_onLoadTextureResponse);
    m_onLoadTextureResponse = nullptr;
}

GenericDto ChangeAvatarTexture::SerializeDto() const
{
    AvatarRecipeChangeTextureDto dto;
    dto.TheFactoryDesc() = m_factoryDesc;
    dto.MeshName() = m_meshName;
    dto.TextureDto() = m_textureDto;
    return dto.ToGenericDto();
}

void ChangeAvatarTexture::Bake(const std::shared_ptr<Pawn>& pawn)
{
    if (!pawn) return;
    if (m_meshName.empty()) return;
    if (m_textureDto.TextureName().empty()) return;
    PrimitivePtr prim = pawn->GetPrimitive();
    if (!prim) return;
    ModelPrimitivePtr model = std::dynamic_pointer_cast<ModelPrimitive, Primitive>(prim);
    if (model)
    {
        auto mesh = model->FindMeshPrimitive(m_meshName);
        if (!mesh) return;
        ChangeMeshTexture(mesh);
    }
    MeshPrimitivePtr mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(prim);
    if (mesh)
    {
        if (mesh->GetName() != m_meshName) return;
        ChangeMeshTexture(mesh);
    }
}

void ChangeAvatarTexture::ChangeMeshTexture(const MeshPrimitivePtr& mesh)
{
    if (!mesh) return;
    if (m_meshName.empty()) return;
    if (m_textureDto.TextureName().empty()) return;
    auto policy = m_textureDto.ConvertToPolicy();
    auto req = std::make_shared<RequestLoadTexture>(std::get<TexturePolicy>(policy));
    m_requsetRuid = req->GetRuid();
    m_mesh = mesh;
    RequestBus::Post(req);
}

void ChangeAvatarTexture::OnLoadTextureResponse(const Frameworks::IResponsePtr& r)
{
    if (!r) return;
    auto res = std::dynamic_pointer_cast<LoadTextureResponse, IResponse>(r);
    if (!res) return;
    if (res->GetRequestRuid() != m_requsetRuid) return;
    if (res->GetErrorCode()) return;
    if (m_mesh.expired()) return;
    m_mesh.lock()->ChangeSemanticTexture({ m_textureDto.Semantic(), res->GetTexture(), m_textureDto.ArrayIndex() });
}
