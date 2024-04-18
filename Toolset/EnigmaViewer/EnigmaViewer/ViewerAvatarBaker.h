/*****************************************************************
 * \file   ViewerAvatarBaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef VIEWER_AVATAR_BAKER_H
#define VIEWER_AVATAR_BAKER_H

#include "Frameworks/CommandSubscriber.h"
#include "GameCommon/AnimatedPawn.h"
#include <functional>

namespace EnigmaViewer
{
    class ViewerAvatarBaker
    {
    public:
        using GetCurrentPawn = std::function<const std::shared_ptr<Enigma::GameCommon::AnimatedPawn>& ()>;
    public:
        ViewerAvatarBaker();
        ViewerAvatarBaker(const ViewerAvatarBaker& other) = delete;
        ViewerAvatarBaker(ViewerAvatarBaker&& other) = delete;
        ~ViewerAvatarBaker();
        ViewerAvatarBaker& operator=(const ViewerAvatarBaker& other) = delete;
        ViewerAvatarBaker& operator=(ViewerAvatarBaker&& other) = delete;

        void subscribeHandlers(const GetCurrentPawn& get_current_pawn);
        void unsubscribeHandlers();

    private:
        void changeMeshTexture(const Enigma::Frameworks::ICommandPtr& c);

    private:
        Enigma::Frameworks::CommandSubscriberPtr m_changeMeshTexture;
        GetCurrentPawn m_getCurrentPawn;
    };
}

#endif // VIEWER_AVATAR_BAKER_H
