/*********************************************************************
 * \file   RenderTextureUsage.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _RENDER_TEXTURE_USAGE_H
#define _RENDER_TEXTURE_USAGE_H

namespace Enigma::Graphics
{
    enum class RenderTextureUsage
    {
        Default,
        Albedo,
        Normal,
        Specular,
        Depth,
        ShadowMap,
    };
}

#endif // _RENDER_TEXTURE_USAGE_H
