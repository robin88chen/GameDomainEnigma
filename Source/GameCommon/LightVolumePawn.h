/*********************************************************************
 * \file   LightVolumePawn.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef LIGHT_VOLUME_PAWN_H
#define LIGHT_VOLUME_PAWN_H

#include "SceneGraph/Pawn.h"

namespace Enigma::GameCommon
{
    class LightVolumePawn : public SceneGraph::Pawn
    {
        DECLARE_EN_RTTI;
    public:
        LightVolumePawn(const std::string& name);
        LightVolumePawn(const Engine::GenericDto& o);
        LightVolumePawn(const LightVolumePawn&) = delete;
        LightVolumePawn(LightVolumePawn&&) = delete;
        virtual ~LightVolumePawn() override;
        LightVolumePawn& operator=(const LightVolumePawn&) = delete;
        LightVolumePawn& operator=(LightVolumePawn&&) = delete;

    };
}

#endif // LIGHT_VOLUME_PAWN_H
