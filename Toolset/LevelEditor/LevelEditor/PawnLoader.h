/*********************************************************************
 * \file   PawnLoader.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PAWN_LOADER_H
#define PAWN_LOADER_H

#include "Frameworks/EventSubscriber.h"
#include <system_error>

namespace LevelEditor
{
    using error = std::error_code;

    class PawnLoader
    {
    public:
        PawnLoader();
        ~PawnLoader();

        error StartLoadingPawn(const std::string& full_path);

    private:
        //void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
        //void OnBuildSceneGraphFailed(const Enigma::Frameworks::IEventPtr& e);

    private:
        //Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        //Enigma::Frameworks::EventSubscriberPtr m_onBuildSceneGraphFailed;

        std::string m_pawnFullPath;
    };
}

#endif // PAWN_LOADER_H
