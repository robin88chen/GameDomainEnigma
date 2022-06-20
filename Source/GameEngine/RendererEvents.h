/*********************************************************************
 * \file   RendererEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDERER_EVENTS_H
#define RENDERER_EVENTS_H

#include "Frameworks/Event.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::Engine
{
    class RenderTargetResized : public Frameworks::IEvent
    {
    public:
        RenderTargetResized(const std::string& name, const MathLib::Dimension& dimension) :
            m_name{ name }, m_dimension(dimension) {};
        const std::string& GetRenderTargetName() { return m_name; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }

    private:
        std::string m_name;
        MathLib::Dimension m_dimension;
    };
}


#endif // RENDERER_EVENTS_H
