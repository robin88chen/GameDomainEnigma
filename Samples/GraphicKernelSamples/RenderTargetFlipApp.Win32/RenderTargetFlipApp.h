/*********************************************************************
 * \file   RenderTargetFlipApp.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDER_TARGET_FLIP_APP_H
#define RENDER_TARGET_FLIP_APP_H

#include "Application/AppDelegateWin32.h"

class RenderTargetFlipApp : public Enigma::Application::AppDelegate
{
public:
    RenderTargetFlipApp(const std::string app_name);

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;
};

#endif // RENDER_TARGET_FLIP_APP_H


