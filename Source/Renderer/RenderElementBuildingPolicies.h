/*********************************************************************
 * \file   RenderElementBuildingPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _RENDER_ELEMEN_TBUILDING_POLICIES_H
#define _RENDER_ELEMEN_TBUILDING_POLICIES_H

#include "GameEngine/RenderBufferSignature.h"
#include "GameEngine/GeometrySegment.h"
#include <string>

namespace Enigma::Renderer
{
    struct RenderElementPolicy
    {
        Engine::RenderBufferSignature m_renderBufferSignature;
        std::string m_effectMaterialName;
        Engine::GeometrySegment m_segment;
    };
}

#endif // _RENDER_ELEMEN_TBUILDING_POLICIES_H
