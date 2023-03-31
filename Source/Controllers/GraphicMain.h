/********************************************************************
 * \file   GraphicMain.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_MAIN_H
#define GRAPHIC_MAIN_H

#include "Frameworks/ServiceManager.h"
#include "InstallingPolicyList.h"
#include <system_error>
#include <cassert>

namespace Enigma::Controllers
{
    using error = std::error_code;

    /** Graphic Kernel Main class \n singleton */
    class GraphicMain
    {
    public:
        GraphicMain();
        GraphicMain(const GraphicMain&) = delete;
        ~GraphicMain();
        GraphicMain& operator=(const GraphicMain&) = delete;

        /** get singleton instance */
        static GraphicMain* Instance();

        error InstallFrameworks();
        error ShutdownFrameworks();

        error InstallRenderEngine(const InstallingPolicyList& policies);
        error ShutdownRenderEngine();

        /** frame update (service manager call run one to update) */
        void FrameUpdate();

        Frameworks::ServiceManager* GetServiceManager() { return m_serviceManager; };

        template <class T>
        std::shared_ptr<T> GetSystemServiceAs()
        {
            assert(m_serviceManager);
            return m_serviceManager->GetSystemServiceAs<T>();
        };

    private:
        static GraphicMain* m_instance;

        Frameworks::ServiceManager* m_serviceManager;

        InstallingPolicyList m_policies;
    };

};


#endif // GRAPHIC_MAIN_H
