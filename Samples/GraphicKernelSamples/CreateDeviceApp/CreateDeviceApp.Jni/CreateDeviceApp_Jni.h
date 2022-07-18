/*********************************************************************
 * \file   CreateDeviceApp_Jni.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef CREATE_DEVICE_APP_JNI_H
#define CREATE_DEVICE_APP_JNI_H


#include "Application/AppDelegateAndroid.h"

class CreateDeviceApp : public Enigma::Application::AppDelegate
{
public:
    CreateDeviceApp(const std::string app_name);

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;
};

#endif // CREATEDEVICEAPP_JNI_H


