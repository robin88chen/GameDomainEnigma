/*********************************************************************
 * \file   DeviceRequiredBits.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef DEVICE_REQUIRED_BITS_H
#define DEVICE_REQUIRED_BITS_H

#include "GraphicAPITypes.h"

namespace Enigma::Graphics
{
    struct DeviceRequiredBits
    {
        unsigned int m_minFullscreenWidth;  /**< min full-screen width */
        unsigned int m_minFullscreenHeight; /**< min full-screen height */
        unsigned int m_minColorChannelBits; /**< min color bits per channel in adapter format */
        unsigned int m_minAlphaChannelBits; /**< min alpha bits per pixel in back buffer format */
        unsigned int m_minDepthBits;                /**< min depth bits */
        unsigned int m_minStencilBits;          /**< min stencil bits */
        bool m_usesDepthBuffer;         /**< use depth buffer?? */
        bool m_usesMixedVP;                 /**< whether app can take advantage of mixed vp mode */
        bool m_usesSoftwareVP;          /**< use software vp mode?? */
        bool m_usesPureHardwareVP;  /**< use pure-hardware vp mode?? */
        bool m_usesHardwareVP;          /**< use hardware vp mode?? */
        bool m_isRequiresWindowed;      /**< windowed mode?? */
        bool m_isRequiresFullscreen;    /**< fullscreen mode?? */
        GraphicFeatureLevel m_featureLevel;

        DeviceRequiredBits()  // default initial values
        {
            m_minFullscreenWidth = 800;
            m_minFullscreenHeight = 600;
            m_minColorChannelBits = 8;  // rgb各5bits以上
            m_minAlphaChannelBits = 8;
            m_minDepthBits = 15;
            m_minStencilBits = 1;
            m_usesDepthBuffer = true;
            m_usesMixedVP = true;
            m_usesSoftwareVP = false;
            m_usesPureHardwareVP = true;
            m_usesHardwareVP = true;
            m_isRequiresWindowed = false;
            m_isRequiresFullscreen = false;
            m_featureLevel = FeatureLevel_11_0;
        }
        Graphics::GraphicFormat GetDepthFormat() const
        {
            return Graphics::GraphicFormat::FMT_D24S8;
        }
    };
}

#endif // DEVICE_REQUIRED_BITS_H
