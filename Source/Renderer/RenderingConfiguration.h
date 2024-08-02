/*********************************************************************
 * \file   RenderingConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef RENDERING_CONFIGURATION_H
#define RENDERING_CONFIGURATION_H

namespace Enigma::Renderer
{
    class IRenderingConfiguration
    {
    public:
        IRenderingConfiguration() = default;
        IRenderingConfiguration(const IRenderingConfiguration&) = delete;
        IRenderingConfiguration(IRenderingConfiguration&&) = delete;
        virtual ~IRenderingConfiguration() = default;
        IRenderingConfiguration& operator=(const IRenderingConfiguration&) = delete;
        IRenderingConfiguration& operator=(IRenderingConfiguration&&) = delete;
    };
}

#endif // RENDERING_CONFIGURATION_H
