/*********************************************************************
 * \file   CreateDeviceApp.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef _CREATE_DEVICE_APP_H
#define _CREATE_DEVICE_APP_H

#include "Application/AppDelegateWin32.h"

class CreateDeviceApp : public Enigma::Application::AppDelegate
{
public:
    CreateDeviceApp(const std::string app_name);

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;
};

#endif // !_CREATE_DEVICE_APP_H
