#include "ViewerAnimationClipCommandHandler.h"
#include "ViewerCommands.h"
#include "Frameworks/CommandBus.h"
#include "GameCommon/AnimationClipMap.h"

using namespace EnigmaViewer;
using namespace Enigma::Frameworks;
using namespace Enigma::GameCommon;

ViewerAnimationClipCommandHandler::ViewerAnimationClipCommandHandler()
{
    m_getCurrentPawn = nullptr;
}

ViewerAnimationClipCommandHandler::~ViewerAnimationClipCommandHandler()
{
    m_getCurrentPawn = nullptr;
}

void ViewerAnimationClipCommandHandler::registerHandlers(const GetCurrentPawn& get_current_pawn)
{
    m_getCurrentPawn = get_current_pawn;
    m_addAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->addAnimationClip(c); });
    CommandBus::subscribe(typeid(AddAnimationClip), m_addAnimationClip);
    m_deleteAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->deleteAnimationClip(c); });
    CommandBus::subscribe(typeid(DeleteAnimationClip), m_deleteAnimationClip);
    m_playAnimationClip = std::make_shared<CommandSubscriber>([=](auto c) { this->playAnimationClip(c); });
    CommandBus::subscribe(typeid(PlayAnimationClip), m_playAnimationClip);
    m_changeAnimationTimeValue = std::make_shared<CommandSubscriber>([=](auto c) { this->changeAnimationTimeValue(c); });
    CommandBus::subscribe(typeid(ChangeAnimationTimeValue), m_changeAnimationTimeValue);
}

void ViewerAnimationClipCommandHandler::unregisterHandlers()
{
    m_getCurrentPawn = nullptr;
    CommandBus::unsubscribe(typeid(AddAnimationClip), m_addAnimationClip);
    m_addAnimationClip = nullptr;
    CommandBus::unsubscribe(typeid(DeleteAnimationClip), m_deleteAnimationClip);
    m_deleteAnimationClip = nullptr;
    CommandBus::unsubscribe(typeid(PlayAnimationClip), m_playAnimationClip);
    m_playAnimationClip = nullptr;
    CommandBus::unsubscribe(typeid(ChangeAnimationTimeValue), m_changeAnimationTimeValue);
    m_changeAnimationTimeValue = nullptr;
}


void ViewerAnimationClipCommandHandler::addAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<AddAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_getCurrentPawn) return;
    auto pawn = m_getCurrentPawn();
    if (!pawn) return;
    if (auto act_clip = pawn->animationClipMap().findAnimationClip(cmd->name()); !act_clip)
    {
        AnimationClipMap::AnimClip act_clip_new(cmd->name(), cmd->clip());
        pawn->animationClipMap().insertClip(act_clip_new);
    }
    else
    {
        act_clip.value().get().changeClip(cmd->clip());
    }
}

void ViewerAnimationClipCommandHandler::deleteAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<DeleteAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_getCurrentPawn) return;
    auto pawn = m_getCurrentPawn();
    if (!pawn) return;
    pawn->animationClipMap().removeClip(cmd->name());
}

void ViewerAnimationClipCommandHandler::playAnimationClip(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<PlayAnimationClip, ICommand>(c);
    if (!cmd) return;
    if (!m_getCurrentPawn) return;
    auto pawn = m_getCurrentPawn();
    if (!pawn) return;
    pawn->playAnimation(cmd->name());
}

void ViewerAnimationClipCommandHandler::changeAnimationTimeValue(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ChangeAnimationTimeValue, ICommand>(c);
    if (!cmd) return;
    if (!m_getCurrentPawn) return;
    auto pawn = m_getCurrentPawn();
    if (!pawn) return;
    bool isNameChanged = false;
    if ((pawn->animationClipMap().findAnimationClip(cmd->oldName()))
        && (cmd->oldName() != cmd->newName()))
    {
        isNameChanged = true;
    }

    if (!isNameChanged)
    {
        if (auto act_clip = pawn->animationClipMap().findAnimationClip(cmd->newName()); act_clip)
        {
            act_clip.value().get().changeClip(cmd->clip());
        }
    }
    else
    {
        pawn->animationClipMap().removeClip(cmd->oldName());
        Enigma::GameCommon::AnimationClipMap::AnimClip act_clip_new(cmd->newName(), cmd->clip());
        pawn->animationClipMap().insertClip(act_clip_new);
    }
}
