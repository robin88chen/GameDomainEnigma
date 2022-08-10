/*********************************************************************
 * \file   GraphicCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef GRAPHIC_COMMANDS_H
#define GRAPHIC_COMMANDS_H

#include "DeviceRequiredBits.h"
#include "Frameworks/Command.h"

namespace Enigma::Graphics
{
    class CreateDevice : public Frameworks::ICommand
    {
    public:
        CreateDevice(const DeviceRequiredBits& rqb, void* hwnd) : m_rqb(rqb), m_hwnd(hwnd) {}
        const DeviceRequiredBits& GetRequiredBits() const { return m_rqb; }
        void* GetHwnd() const { return m_hwnd; }

    private:
        DeviceRequiredBits m_rqb;
        void* m_hwnd;
    };
    class CleanupDevice : public Frameworks::ICommand
    {
    };
}


#endif // GRAPHIC_COMMANDS_H
