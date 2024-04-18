/*****************************************************************
 * \file   ViewerAnimationClipCommandHandler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef VIEWER_ANIMATION_CLIP_COMMAND_HANDLER_H
#define VIEWER_ANIMATION_CLIP_COMMAND_HANDLER_H

#include "Frameworks/CommandSubscriber.h"
#include "GameCommon/AnimatedPawn.h"

namespace EnigmaViewer
{
    class ViewerAnimationClipCommandHandler
    {
    public:
        using GetCurrentPawn = std::function<const std::shared_ptr<Enigma::GameCommon::AnimatedPawn>& ()>;
    public:
        ViewerAnimationClipCommandHandler();
        ~ViewerAnimationClipCommandHandler();
        ViewerAnimationClipCommandHandler(const ViewerAnimationClipCommandHandler&) = delete;
        ViewerAnimationClipCommandHandler(ViewerAnimationClipCommandHandler&&) = delete;
        ViewerAnimationClipCommandHandler& operator=(const ViewerAnimationClipCommandHandler&) = delete;
        ViewerAnimationClipCommandHandler& operator=(ViewerAnimationClipCommandHandler&&) = delete;

        void registerHandlers(const GetCurrentPawn& get_current_pawn);
        void unregisterHandlers();

    private:
        void addAnimationClip(const Enigma::Frameworks::ICommandPtr& c);
        void deleteAnimationClip(const Enigma::Frameworks::ICommandPtr& c);
        void playAnimationClip(const Enigma::Frameworks::ICommandPtr& c);
        void changeAnimationTimeValue(const Enigma::Frameworks::ICommandPtr& c);

    private:
        GetCurrentPawn m_getCurrentPawn;

        Enigma::Frameworks::CommandSubscriberPtr m_addAnimationClip;
        Enigma::Frameworks::CommandSubscriberPtr m_deleteAnimationClip;
        Enigma::Frameworks::CommandSubscriberPtr m_playAnimationClip;
        Enigma::Frameworks::CommandSubscriberPtr m_changeAnimationTimeValue;
    };
}

#endif // VIEWER_ANIMATION_CLIP_COMMAND_HANDLER_H
