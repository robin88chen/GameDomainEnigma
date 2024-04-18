#include "ViewerAvatarBaker.h"
#include "ViewerCommands.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameCommon/AvatarRecipes.h"

using namespace EnigmaViewer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::GameCommon;

ViewerAvatarBaker::ViewerAvatarBaker()
{
    m_getCurrentPawn = nullptr;
}

ViewerAvatarBaker::~ViewerAvatarBaker()
{
    m_getCurrentPawn = nullptr;
}

void ViewerAvatarBaker::subscribeHandlers(const GetCurrentPawn& get_current_pawn)
{
    m_getCurrentPawn = get_current_pawn;
    m_changeMeshTexture = std::make_shared<CommandSubscriber>([=](auto c) { this->changeMeshTexture(c); });
    CommandBus::subscribe(typeid(ChangeMeshTexture), m_changeMeshTexture);
}

void ViewerAvatarBaker::unsubscribeHandlers()
{
    CommandBus::unsubscribe(typeid(ChangeMeshTexture), m_changeMeshTexture);
    m_changeMeshTexture = nullptr;
    m_getCurrentPawn = nullptr;
}

void ViewerAvatarBaker::changeMeshTexture(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ChangeMeshTexture, ICommand>(c);
    if (!cmd) return;
    if (!m_getCurrentPawn) return;
    auto pawn = m_getCurrentPawn();
    if (!pawn) return;
    TextureMappingDto tex_dto;
    tex_dto.textureId() = cmd->textureId();
    tex_dto.semantic() = "DiffuseMap";
    auto recipe = std::make_shared<ChangeAvatarTexture>(cmd->meshId(), tex_dto);
    pawn->addAvatarRecipe(recipe);
    pawn->bakeAvatarRecipes();
}

