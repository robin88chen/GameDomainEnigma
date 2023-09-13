#include "AvatarRecipes.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "GameEngine/EffectCommands.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/TextureRequests.h"
#include "GameEngine/TextureResponses.h"
#include "Renderer/ModelPrimitive.h"
#include "AvatarRecipeDto.h"
#include "Platforms/PlatformLayer.h"

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
    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::Subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    EventPublisher::Subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
}

ReplaceAvatarMaterial::ReplaceAvatarMaterial(const Engine::GenericDto& o) : AvatarRecipe(o)
{
    AvatarRecipeReplaceMaterialDto dto = AvatarRecipeReplaceMaterialDto::FromGenericDto(o);
    m_oldMaterialName = dto.OldMaterialName();
    m_newMaterialDto = dto.NewMaterialDto();
    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::Subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    EventPublisher::Subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
}

ReplaceAvatarMaterial::~ReplaceAvatarMaterial()
{
    EventPublisher::Unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    EventPublisher::Unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;
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
            auto cmd = std::make_shared<CompileEffectMaterial>(m_newMaterialDto);
            m_changeSpecifyMaterialMap[cmd->GetRuid()] = [mesh, i, this](const EffectMaterialPtr& eff_new)
                {
                    mesh->ChangeEffectMaterialInSegment(i, eff_new);
                    mesh->CreateRenderElements();
                    if (!m_primitive.expired())
                    {
                        m_primitive.lock()->SelectVisualTechnique(m_primitive.lock()->GetSelectedVisualTechnique());
                    }
                };
            CommandBus::Post(cmd);
        }
    }
}

void ReplaceAvatarMaterial::OnEffectMaterialCompiled(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled>(e);
    if (!ev) return;
    auto it = m_changeSpecifyMaterialMap.find(ev->GetRequestRuid());
    if (it == m_changeSpecifyMaterialMap.end()) return;
    it->second(ev->GetEffect());
}

void ReplaceAvatarMaterial::OnCompileEffectMaterialFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("ReplaceAvatarMaterial::OnCompileEffectMaterialFailed: %s, %s\n", ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
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
