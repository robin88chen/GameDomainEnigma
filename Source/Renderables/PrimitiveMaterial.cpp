#include "PrimitiveMaterial.h"
#include "PrimitiveMaterialAssembler.h"
#include "RenderableErrors.h"
#include "RenderableEvents.h"
#include "GameEngine/Texture.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/EffectSemanticTexture.h"
#include "GameEngine/TextureEvents.h"

using namespace Enigma::Renderables;

DEFINE_RTTI_OF_BASE(Renderables, PrimitiveMaterial);

PrimitiveMaterial::PrimitiveMaterial() : m_effectMaterial(nullptr), m_effectTextureMap()
{
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
}

PrimitiveMaterial::PrimitiveMaterial(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial, const Engine::EffectTextureMap& effectTextureMap) : m_effectMaterial(effectMaterial), m_effectTextureMap(effectTextureMap)
{
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
    changeWhetherReady();
}

PrimitiveMaterial::PrimitiveMaterial(const Engine::EffectMaterialId& effectMaterialId, const Engine::EffectTextureMap& effectTextureMap)
{
    m_effectMaterial = Engine::EffectMaterial::queryEffectMaterial(effectMaterialId);
    m_effectTextureMap = effectTextureMap;
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
    changeWhetherReady();
}

PrimitiveMaterial::~PrimitiveMaterial()
{
    unregisterHydrationEventHandlers();
}

std::shared_ptr<PrimitiveMaterialAssembler> PrimitiveMaterial::assembler() const
{
    return std::make_shared<PrimitiveMaterialAssembler>(m_effectMaterial, m_effectTextureMap);
}

void PrimitiveMaterial::assemble(const std::shared_ptr<PrimitiveMaterialAssembler>& assembler) const
{
    assert(assembler);
    assembler->effectMaterialId(m_effectMaterial->id());
    assembler->effectTextureMap(m_effectTextureMap);
}

std::shared_ptr<PrimitiveMaterialDisassembler> PrimitiveMaterial::disassembler() const
{
    return std::make_shared<PrimitiveMaterialDisassembler>();
}

void PrimitiveMaterial::disassemble(const std::shared_ptr<PrimitiveMaterialDisassembler>& disassembler)
{
    assert(disassembler);
    m_effectMaterial = Engine::EffectMaterial::queryEffectMaterial(disassembler->effectMaterialId());
    m_effectTextureMap = disassembler->effectTextureMap();
    changeWhetherReady();
    registerEventHandlersWhenNotReady();
}

void PrimitiveMaterial::registerEventHandlersWhenNotReady()
{
    if (!m_lazyStatus.isReady())
    {
        registerHydrationEventHandlers();
    }
}

std::error_code PrimitiveMaterial::assignShaderTextures()
{
    if (m_effectMaterial == nullptr) return ErrorCode::nullEffectMaterial;
    for (unsigned i = 0; i < m_effectTextureMap.getCount(); i++)
    {
        auto& eff_tex = m_effectTextureMap.getEffectSemanticTexture(i);
        if (eff_tex.texture() == nullptr) continue;
        // 改直接指定
        m_effectMaterial->assignVariableValue(eff_tex.semantic(), Graphics::IShaderVariable::TextureVarTuple{
                    eff_tex.texture()->getDeviceTexture(), eff_tex.arrayIndex() });
        /*(*eff_iter)->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{
                    std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            });*/
    }
    return ErrorCode::ok;
}

std::error_code PrimitiveMaterial::unassignShaderTextures()
{
    if (m_effectMaterial == nullptr) return ErrorCode::nullEffectMaterial;
    for (unsigned i = 0; i < m_effectTextureMap.getCount(); i++)
    {
        auto& eff_tex = m_effectTextureMap.getEffectSemanticTexture(i);
        // 改直接指定
        m_effectMaterial->assignVariableValue(eff_tex.semantic(), Graphics::IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
        /*(*eff_iter)->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            });*/
    }
    return ErrorCode::ok;
}

std::error_code PrimitiveMaterial::assignVariableFunc(const std::string& semantic, Engine::EffectVariable::VariableValueAssignFunc fn)
{
    if (m_effectMaterial == nullptr) return ErrorCode::nullEffectMaterial;
    m_effectMaterial->setVariableAssignFunc(semantic, fn);
    return ErrorCode::ok;
}

void PrimitiveMaterial::changeSemanticTexture(const Engine::EffectSemanticTexture& semantic_texture)
{
    m_effectTextureMap.changeSemanticTexture(semantic_texture);
}

void PrimitiveMaterial::bindSemanticTexture(const Engine::EffectSemanticTexture& semantic_texture)
{
    m_effectTextureMap.bindSemanticTexture(semantic_texture);
}

void PrimitiveMaterial::bindSemanticTextures(const std::vector<Engine::EffectSemanticTexture>& textures)
{
    for (const auto& tex : textures)
    {
        bindSemanticTexture(tex);
    }
}

void PrimitiveMaterial::changeEffect(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial)
{
    m_effectMaterial = effectMaterial;
}

void PrimitiveMaterial::changeTextureMap(const Engine::EffectTextureMap& effectTextureMap)
{
    m_effectTextureMap = effectTextureMap;
}

void PrimitiveMaterial::selectVisualTechnique(const std::string& techniqueName)
{
    if (m_effectMaterial) m_effectMaterial->selectVisualTechnique(techniqueName);
}

void PrimitiveMaterial::changeWhetherReady()
{
    if ((m_effectMaterial) && (m_effectMaterial->lazyStatus().isReady()) && m_effectTextureMap.isAllTextureReady())
    {
        m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ready);
    }
}

bool PrimitiveMaterial::shouldWaitHydration()
{
    return (m_effectMaterial == nullptr) || (!m_effectMaterial->lazyStatus().isReady()) || (!m_effectTextureMap.isAllTextureReady());
}

void PrimitiveMaterial::registerHydrationEventHandlers()
{
    m_onEffectHydrated = Frameworks::eventSubscription(typeid(Engine::EffectMaterialHydrated), [=](const Frameworks::IEventPtr& e) { onEffectHydrated(e); });
    m_onEffectHydrationFailed = Frameworks::eventSubscription(typeid(Engine::HydrateEffectMaterialFailed), [=](const Frameworks::IEventPtr& e) { onEffectHydrationFailed(e); });
    m_onTextureHydrated = Frameworks::eventSubscription(typeid(Engine::TextureHydrated), [=](const Frameworks::IEventPtr& e) { onTextureHydrated(e); });
    m_onTextureHydrationFailed = Frameworks::eventSubscription(typeid(Engine::HydrateTextureFailed), [=](const Frameworks::IEventPtr& e) { onTextureHydrationFailed(e); });
}

void PrimitiveMaterial::unregisterHydrationEventHandlers()
{
    Frameworks::releaseSubscription(typeid(Engine::EffectMaterialHydrated), m_onEffectHydrated);
    m_onEffectHydrated = nullptr;
    Frameworks::releaseSubscription(typeid(Engine::HydrateEffectMaterialFailed), m_onEffectHydrationFailed);
    m_onEffectHydrationFailed = nullptr;
    Frameworks::releaseSubscription(typeid(Engine::TextureHydrated), m_onTextureHydrated);
    m_onTextureHydrated = nullptr;
    Frameworks::releaseSubscription(typeid(Engine::HydrateTextureFailed), m_onTextureHydrationFailed);
    m_onTextureHydrationFailed = nullptr;
}

void PrimitiveMaterial::onEffectHydrated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Engine::EffectMaterialHydrated>(e);
    if (!ev) return;

    changeWhetherReady();
    if (m_lazyStatus.isReady())
    {
        unregisterHydrationEventHandlers();
        std::make_shared<PrimitiveMaterialHydrated>(shared_from_this())->enqueue();
    }
}

void PrimitiveMaterial::onEffectHydrationFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Engine::HydrateEffectMaterialFailed>(e);
    if (!ev) return;
    if (ev->id() != m_effectMaterial->id()) return;
    std::make_shared<PrimitiveMaterialHydrationFailed>(shared_from_this(), ev->error())->enqueue();
}

void PrimitiveMaterial::onTextureHydrated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Engine::TextureHydrated>(e);
    if (!ev) return;

    changeWhetherReady();
    if (m_lazyStatus.isReady())
    {
        unregisterHydrationEventHandlers();
        std::make_shared<PrimitiveMaterialHydrated>(shared_from_this())->enqueue();
    }
}

void PrimitiveMaterial::onTextureHydrationFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Engine::HydrateTextureFailed>(e);
    if (!ev) return;
    if (m_effectTextureMap.hasTexture(ev->id()))
    {
        std::make_shared<PrimitiveMaterialHydrationFailed>(shared_from_this(), ev->error())->enqueue();
    }
}
