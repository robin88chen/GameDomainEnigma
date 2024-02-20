#include "AvatarRecipeDto.h"
#include "AvatarRecipes.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/QueryDispatcher.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/EffectQueries.h"
#include "GameEngine/Texture.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/TextureQueries.h"
#include "Platforms/PlatformLayer.h"
#include "Renderables/ModelPrimitive.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(GameCommon, AvatarRecipe);
DEFINE_RTTI(GameCommon, ReplaceAvatarMaterial, AvatarRecipe);
DEFINE_RTTI(GameCommon, ChangeAvatarTexture, AvatarRecipe);

AvatarRecipe::AvatarRecipe() : m_factoryDesc(AvatarRecipe::TYPE_RTTI.getName())
{
}

AvatarRecipe::AvatarRecipe(const Engine::GenericDto& o) : m_factoryDesc(o.getRtti())
{
}

std::shared_ptr<AvatarRecipe> AvatarRecipe::createFromGenericDto(const Engine::GenericDto& dto)
{
    std::string type = dto.getRtti().GetRttiName();
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


ReplaceAvatarMaterial::ReplaceAvatarMaterial(const EffectMaterialId& old_material_id, const EffectMaterialId& new_material_id)
    : m_oldMaterialId(old_material_id), m_newMaterialId(new_material_id)
{
    m_factoryDesc = FactoryDesc(ReplaceAvatarMaterial::TYPE_RTTI.getName());
    m_onEffectMaterialHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onEffectMaterialHydrated(e); });
    EventPublisher::subscribe(typeid(EffectMaterialHydrated), m_onEffectMaterialHydrated);
    m_onHydrateEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydrateMaterialFailed(e); });
    EventPublisher::subscribe(typeid(HydrateEffectMaterialFailed), m_onHydrateEffectMaterialFailed);
}

ReplaceAvatarMaterial::ReplaceAvatarMaterial(const Engine::GenericDto& o) : AvatarRecipe(o)
{
    AvatarRecipeReplaceMaterialDto dto = AvatarRecipeReplaceMaterialDto::fromGenericDto(o);
    m_oldMaterialId = dto.oldMaterialId();
    m_newMaterialId = dto.newMaterialId();
    m_onEffectMaterialHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onEffectMaterialHydrated(e); });
    EventPublisher::subscribe(typeid(EffectMaterialHydrated), m_onEffectMaterialHydrated);
    m_onHydrateEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydrateMaterialFailed(e); });
    EventPublisher::subscribe(typeid(HydrateEffectMaterialFailed), m_onHydrateEffectMaterialFailed);
}

ReplaceAvatarMaterial::~ReplaceAvatarMaterial()
{
    EventPublisher::unsubscribe(typeid(EffectMaterialHydrated), m_onEffectMaterialHydrated);
    m_onEffectMaterialHydrated = nullptr;
    EventPublisher::unsubscribe(typeid(HydrateEffectMaterialFailed), m_onHydrateEffectMaterialFailed);
    m_onHydrateEffectMaterialFailed = nullptr;
    m_changeSpecifyMaterialMap.clear();
}

GenericDto ReplaceAvatarMaterial::serializeDto() const
{
    AvatarRecipeReplaceMaterialDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.oldMaterialId() = m_oldMaterialId;
    dto.newMaterialId() = m_newMaterialId;
    return dto.toGenericDto();
}

void ReplaceAvatarMaterial::bake(const std::shared_ptr<Pawn>& pawn)
{
    if (!pawn) return;
    if (m_oldMaterialId.isEqualSource(m_newMaterialId)) return;
    std::shared_ptr<Primitives::Primitive> prim = pawn->getPrimitive();
    if (!prim) return;
    m_primitive = prim;
    std::shared_ptr<ModelPrimitive> model = std::dynamic_pointer_cast<ModelPrimitive>(prim);
    if (model)
    {
        unsigned int total_mesh_prim = model->getMeshPrimitiveCount();
        for (unsigned int i = 0; i < total_mesh_prim; i++)
        {
            std::shared_ptr<MeshPrimitive> mesh = model->getMeshPrimitive(i);
            if (mesh)
            {
                replaceMeshMaterial(mesh);
            }
        }
    }
    std::shared_ptr<MeshPrimitive> mesh = std::dynamic_pointer_cast<MeshPrimitive>(prim);
    if (mesh)
    {
        replaceMeshMaterial(mesh);
    }
}

void ReplaceAvatarMaterial::replaceMeshMaterial(const std::shared_ptr<MeshPrimitive>& mesh)
{
    if (!mesh) return;
    if (m_oldMaterialId.isEqualSource(m_newMaterialId)) return;

    unsigned int total_mat_count = mesh->getEffectMaterialCount();
    if (total_mat_count == 0) return;
    for (unsigned int i = 0; i < total_mat_count; i++)
    {
        std::shared_ptr<EffectMaterial> eff = mesh->getEffectMaterial(i);
        if (!eff) continue;
        if (eff->id().isEqualSource(m_oldMaterialId))
        {
            auto query = std::make_shared<QueryEffectMaterial>(m_newMaterialId);
            QueryDispatcher::dispatch(query);
            if (auto eff_new = query->getResult())
            {
                if (eff_new->lazyStatus().isReady())
                {
                    replaceNewMeshMaterialInSegment(mesh, i, eff_new);
                }
                else
                {
                    m_changeSpecifyMaterialMap[eff_new->id()] = [=](const EffectMaterialId& id)
                        {
                            if (id == eff_new->id()) replaceNewMeshMaterialInSegment(mesh, i, eff_new);
                        };
                }
            }
            else
            {
                Platforms::Debug::ErrorPrintf("ReplaceAvatarMaterial::ReplaceMeshMaterial: cannot find new material %s\n", m_newMaterialId.name().c_str());
            }
        }
    }
}

void ReplaceAvatarMaterial::replaceNewMeshMaterialInSegment(const std::shared_ptr<MeshPrimitive>& mesh, unsigned segment_index, const std::shared_ptr<EffectMaterial>& new_material)
{
    if (!mesh) return;
    if (!new_material) return;
    if (segment_index >= mesh->getEffectMaterialCount()) return;
    mesh->changeEffectMaterialInSegment(segment_index, new_material);
    mesh->createRenderElements();
    if (!m_primitive.expired())
    {
        m_primitive.lock()->selectVisualTechnique(m_primitive.lock()->getSelectedVisualTechnique());
    }
}

void ReplaceAvatarMaterial::onEffectMaterialHydrated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialHydrated>(e);
    if (!ev) return;
    auto it = m_changeSpecifyMaterialMap.find(ev->id());
    if (it == m_changeSpecifyMaterialMap.end()) return;
    it->second(ev->id());
}

void ReplaceAvatarMaterial::onHydrateMaterialFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<HydrateEffectMaterialFailed>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("ReplaceAvatarMaterial::OnContentEffectMaterialFailed: %s, %s\n", ev->id().name().c_str(), ev->error().message().c_str());
}

ChangeAvatarTexture::ChangeAvatarTexture(const std::string& mesh_name, const TextureMappingDto& texture_dto)
    : m_meshName(mesh_name), m_textureDto(texture_dto)
{
    m_factoryDesc = FactoryDesc(ChangeAvatarTexture::TYPE_RTTI.getName());
    m_onTextureHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onTextureHydrated(e); });
    EventPublisher::subscribe(typeid(TextureHydrated), m_onTextureHydrated);
    m_onHydrateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydrateTextureFailed(e); });
    EventPublisher::subscribe(typeid(HydrateTextureFailed), m_onTextureHydrated);
}

ChangeAvatarTexture::ChangeAvatarTexture(const Engine::GenericDto& o) : AvatarRecipe(o)
{
    AvatarRecipeChangeTextureDto dto = AvatarRecipeChangeTextureDto::fromGenericDto(o);
    m_meshName = dto.MeshName();
    m_textureDto = dto.TextureDto();
    m_onTextureHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onTextureHydrated(e); });
    EventPublisher::subscribe(typeid(TextureHydrated), m_onTextureHydrated);
    m_onHydrateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydrateTextureFailed(e); });
    EventPublisher::subscribe(typeid(HydrateTextureFailed), m_onHydrateTextureFailed);
}

ChangeAvatarTexture::~ChangeAvatarTexture()
{
    EventPublisher::unsubscribe(typeid(TextureHydrated), m_onTextureHydrated);
    m_onTextureHydrated = nullptr;
    EventPublisher::unsubscribe(typeid(HydrateTextureFailed), m_onHydrateTextureFailed);
    m_onHydrateTextureFailed = nullptr;
}

GenericDto ChangeAvatarTexture::serializeDto() const
{
    AvatarRecipeChangeTextureDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.MeshName() = m_meshName;
    dto.TextureDto() = m_textureDto;
    return dto.toGenericDto();
}

void ChangeAvatarTexture::bake(const std::shared_ptr<Pawn>& pawn)
{
    if (!pawn) return;
    if (m_meshName.empty()) return;
    std::shared_ptr<Primitives::Primitive> prim = pawn->getPrimitive();
    if (!prim) return;
    std::shared_ptr<ModelPrimitive> model = std::dynamic_pointer_cast<ModelPrimitive>(prim);
    if (model)
    {
        auto mesh = model->findMeshPrimitive(m_meshName);
        if (!mesh) return;
        changeMeshTexture(mesh);
    }
    std::shared_ptr<MeshPrimitive> mesh = std::dynamic_pointer_cast<MeshPrimitive>(prim);
    if (mesh)
    {
        if (mesh->getName() != m_meshName) return;
        changeMeshTexture(mesh);
    }
}

void ChangeAvatarTexture::changeMeshTexture(const std::shared_ptr<MeshPrimitive>& mesh)
{
    if (!mesh) return;
    if (m_meshName.empty()) return;
    auto query = std::make_shared<QueryTexture>(m_textureDto.textureId());
    QueryDispatcher::dispatch(query);
    if ((query->getResult()) && (query->getResult()->lazyStatus().isReady()))
    {
        mesh->changeSemanticTexture({ m_textureDto.semantic(), query->getResult(), m_textureDto.arrayIndex() });
    }
    else
    {
        m_mesh = mesh;
    }
}

void ChangeAvatarTexture::onTextureHydrated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureHydrated>(e);
    if (!ev) return;
    if (ev->id() != m_textureDto.textureId()) return;
    if (m_mesh.expired()) return;
    m_mesh.lock()->changeSemanticTexture({ m_textureDto.semantic(), ev->texture(), m_textureDto.arrayIndex() });
}

void ChangeAvatarTexture::onHydrateTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<HydrateTextureFailed>(e);
    if (!ev) return;
    if (ev->id() != m_textureDto.textureId()) return;
    Platforms::Debug::ErrorPrintf("ChangeAvatarTexture::OnLoadTextureFailed: %s, %s\n", ev->id().name().c_str(), ev->error().message().c_str());
}

