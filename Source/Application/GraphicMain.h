/********************************************************************
 * \file   GraphicMain.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_MAIN_H
#define GRAPHIC_MAIN_H

#include <system_error>
#include "ServiceManager.h"
#include "IGraphicAPI.h"

namespace Enigma::Application
{
    using error = std::error_code;
    /** Graphic Kernel Main class \n singleton */
    class GraphicMain
    {
    public:
        enum class GraphicCoordSys
        {
            LeftHand,
            RightHand
        };
    public:
        GraphicMain(Graphics::IGraphicAPI::AsyncType useAsyncDevice = Graphics::IGraphicAPI::AsyncType::UseAsyncDevice, 
            GraphicCoordSys coordSys = GraphicCoordSys::LeftHand);
        GraphicMain(const GraphicMain&) = delete;
        ~GraphicMain();
        GraphicMain& operator=(const GraphicMain&) = delete;

        /** get singleton instance */
        static GraphicMain* Instance();

        error InstallFrameworks();
        error ShutdownFrameworks();

        /** frame update (service manager call run one to update) */
        void FrameUpdate();

        Frameworks::ServiceManager* GetServiceManager() { return m_serviceManager; };

    private:
        static GraphicMain* m_instance;

        GraphicCoordSys m_coordSys;
        Graphics::IGraphicAPI::AsyncType m_asyncType;

        Frameworks::ServiceManager* m_serviceManager;
    };

};


#endif // GRAPHIC_MAIN_H
