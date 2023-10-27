/*********************************************************************
 * \file   LightEditService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LIGHT_EDIT_SERVICE_H
#define LIGHT_EDIT_SERVICE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"

namespace LevelEditor
{
    class LightEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        LightEditService(Enigma::Frameworks::ServiceManager* srv_mngr);
        LightEditService(const LightEditService&) = delete;
        LightEditService& operator=(const LightEditService&) = delete;
        LightEditService(LightEditService&&) = delete;
        LightEditService& operator=(LightEditService&&) = delete;
        virtual ~LightEditService() override;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

    protected:
        void DoCreatingEnvironmentLight(const Enigma::Frameworks::ICommandPtr& c);

    protected:
        Enigma::Frameworks::CommandSubscriberPtr m_doCreatingEnvironmentLight;
    };
}

#endif // LIGHT_EDIT_SERVICE_H
