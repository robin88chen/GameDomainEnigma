/*********************************************************************
 * \file   GraphicEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_EVENTS_H
#define GRAPHIC_EVENTS_H

#include "Frameworks/Event.h"

namespace Enigma::Graphics
{
    class DeviceCreated : public Frameworks::IEvent
    {
    };
    class PrimarySurfaceCreated : public Frameworks::IEvent
    {
    public:
        PrimarySurfaceCreated(const std::string& back_name, const std::string& depth_name) :
            m_backSurfaceName(back_name), m_depthSurfaceName(depth_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_backSurfaceName;
        std::string m_depthSurfaceName;
    };
    class BackSurfaceCreated : public Frameworks::IEvent
    {
    public:
        BackSurfaceCreated(const std::string& back_name) :
            m_backSurfaceName(back_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_backSurfaceName;
    };
    class MultiBackSurfaceCreated : public Frameworks::IEvent
    {
    public:
        MultiBackSurfaceCreated(const std::string& back_name) :
            m_backSurfaceName(back_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_backSurfaceName;
    };
    class DepthSurfaceCreated : public Frameworks::IEvent
    {
    public:
        DepthSurfaceCreated(const std::string& depth_name) :
            m_depthSurfaceName(depth_name) {};
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_depthSurfaceName;
    };
    class DepthSurfaceShared : public Frameworks::IEvent
    {
    public:
        DepthSurfaceShared(const std::string& depth_name) :
            m_depthSurfaceName(depth_name) {};
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_depthSurfaceName;
    };
}

#endif // GRAPHIC_EVENTS_H
