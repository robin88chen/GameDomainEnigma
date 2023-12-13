#include "AvatarRecipes.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/ResponseBus.h"
#include "GameEngine/EffectCommands.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
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

AvatarRecipe::AvatarRecipe() : m_factoryDesc(AvatarRecipe::TYPE_RTTI.getName())
{
}

AvatarRecipe::AvatarRecipe(const Engine::GenericDto& o) : m_factoryDesc(o.GetRtti())
{
}

std::shared_ptr<AvatarRecipe> AvatarRecipe::CreateFromGenericDto(const Engine::GenericDto& dto)
{
    std::string type = dto.GetRtti().GetRttiName();
    if (type == ReplaceAvatarMaterial::TYPE_RTTI.getName())
    {
        return std::make_shared<ReplaceAvatarMaterial>(dto);
    }
    else if (type == ChangeAvatarTexture::TYPE_RTTI.getName())
    {
        return std::make_shared<ChangeAvatarTexture>(dto);
    }
    return nullptr;
}


ReplaceAvatarMaterial::ReplaceAvatarMaterial(const std::string& old_material_name, const EffectMaterialDto& new_material_dto)
    : m_oldMaterialName(old_material_name), m_newMaterialDto(new_material_dto)
{
    m_factoryDesc = FactoryDesc(ReplaceAvatarMaterial::TYPE_RTTI.getName());
    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
}

ReplaceAvatarMaterial::ReplaceAvatarMaterial(const Engine::GenericDto& o) : AvatarRecipe(o)
{
    AvatarRecipeReplaceMaterialDto dto = AvatarRecipeReplaceMaterialDto::fromGenericDto(o);
    m_oldMaterialName = dto.OldMaterialName();
    m_newMaterialDto = dto.NewMaterialDto();
    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
}

ReplaceAvatarMaterial::~ReplaceAvatarMaterial()
{
    EventPublisher::unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    EventPublisher::unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;
    m_changeSpecifyMaterialMap.clear();
}

GenericDto ReplaceAvatarMaterial::serializeDto() const
{
    AvatarRecipeReplaceMaterialDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.OldMaterialName() = m_oldMaterialName;
    dto.NewMaterialDto() = m_newMaterialDto;
    return dto.toGenericDto();
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

    unsigned int total_mat_count = mesh->getEffectMaterialCount();
    if (total_mat_count == 0) return;
    for (unsigned int i = 0; i < total_mat_count; i++)
    {
        EffectMaterialPtr eff = mesh->getEffectMaterial(i);
        if (!eff) continue;
        if (eff->getName() == m_oldMaterialName)
        {
            //todo: 先丟 request
            auto cmd = std::make_shared<CompileEffectMaterial>(m_newMaterialDto);
            m_changeSpecifyMaterialMap[cmd->getRuid()] = [mesh, i, this](const EffectMaterialPtr& eff_new)
                {
                    mesh->changeEffectMaterialInSegment(i, eff_new);
                    mesh->createRenderElements();
                    if (!m_primitive.expired())
                    {
                        m_primitive.lock()->selectVisualTechnique(m_primitive.lock()->getSelectedVisualTechnique());
                    }
                };
            CommandBus::post(cmd);
        }
    }
}

void ReplaceAvatarMaterial::OnEffectMaterialCompiled(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled>(e);
    if (!ev) return;
    auto it = m_changeSpecifyMaterialMap.find(ev->getRequestRuid());
    if (it == m_changeSpecifyMaterialMap.end()) return;
    it->second(ev->GetEffect());
}

void ReplaceAvatarMaterial::OnCompileEffectMaterialFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("ReplaceAvatarMaterial::OnCompileEffectMaterialFailed: %s, %s\n", ev->getName().c_str(), ev->GetErrorCode().message().c_str());
}

ChangeAvatarTexture::ChangeAvatarTexture(const std::string& mesh_name, const TextureMappingDto& texture_dto)
    : m_meshName(mesh_name), m_textureDto(texture_dto), m_requsetRuid()
{
    m_factoryDesc = FactoryDesc(ChangeAvatarTexture::TYPE_RTTI.getName());
    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoaded(e); });
    EventPublisher::subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadTextureFailed(e); });
    EventPublisher::subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
}

ChangeAvatarTexture::ChangeAvatarTexture(const Engine::GenericDto& o) : AvatarRecipe(o), m_requsetRuid()
{
    AvatarRecipeChangeTextureDto dto = AvatarRecipeChangeTextureDto::fromGenericDto(o);
    m_meshName = dto.MeshName();
    m_textureDto = dto.TextureDto();
    EventPublisher::subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadTextureFailed(e); });
    EventPublisher::subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
}

ChangeAvatarTexture::~ChangeAvatarTexture()
{
    EventPublisher::unsubscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    EventPublisher::unsubscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;
}

GenericDto ChangeAvatarTexture::serializeDto() const
{
    AvatarRecipeChangeTextureDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.MeshName() = m_meshName;
    dto.TextureDto() = m_textureDto;
    return dto.toGenericDto();
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
        if (mesh->getName() != m_meshName) return;
        ChangeMeshTexture(mesh);
    }
}

void ChangeAvatarTexture::ChangeMeshTexture(const MeshPrimitivePtr& mesh)
{
    if (!mesh) return;
    if (m_meshName.empty()) return;
    if (m_textureDto.TextureName().empty()) return;
    auto policy = m_textureDto.ConvertToPolicy();
    auto req = std::make_shared<LoadTexture>(std::get<TexturePolicy>(policy));
    m_requsetRuid = req->getRuid();
    m_mesh = mesh;
    CommandBus::post(req);
}

void ChangeAvatarTexture::OnTextureLoaded(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureLoaded>(e);
    if (!ev) return;
    if (ev->getRequestRuid() != m_requsetRuid) return;
    if (m_mesh.expired()) return;
    m_mesh.lock()->changeSemanticTexture({ m_textureDto.Semantic(), ev->GetTexture(), m_textureDto.ArrayIndex() });
}

void ChangeAvatarTexture::OnLoadTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LoadTextureFailed>(e);
    if (!ev) return;
    if (ev->getRequestRuid() != m_requsetRuid) return;
    Platforms::Debug::ErrorPrintf("ChangeAvatarTexture::OnLoadTextureFailed: %s, %s\n", ev->getName().c_str(), ev->GetErrorCode().message().c_str());
}

