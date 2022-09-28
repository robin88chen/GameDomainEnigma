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
#include <memory>
#include "Frameworks/ServiceManager.h"
#include "GraphicKernel/IGraphicAPI.h"

namespace Enigma::Renderer
{
    class RendererManager;
};

namespace Enigma::Controllers
{
    using error = std::error_code;
    class InstallingPolicy;
    class DeviceCreatingPolicy;
    class InstallingDefaultRendererPolicy;

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
        GraphicMain(GraphicCoordSys coordSys = GraphicCoordSys::LeftHand);
        GraphicMain(const GraphicMain&) = delete;
        ~GraphicMain();
        GraphicMain& operator=(const GraphicMain&) = delete;

        /** get singleton instance */
        static GraphicMain* Instance();

        error InstallFrameworks();
        error ShutdownFrameworks();

        error InstallRenderEngine(std::unique_ptr<InstallingPolicy> policy);
        error ShutdownRenderEngine();

        /** frame update (service manager call run one to update) */
        void FrameUpdate();

        Frameworks::ServiceManager* GetServiceManager() { return m_serviceManager; };

    private:
        error CreateRenderEngineDevice(DeviceCreatingPolicy* policy);
        error CleanupRenderEngineDevice();

        error InstallDefaultRenderer(InstallingDefaultRendererPolicy* policy);
        error ShutdownDefaultRenderer();

        error InstallRenderer(const std::string& renderer_name, const std::string render_target_name, bool is_primary);
        error ShutdownRenderer(const std::string& renderer_name, const std::string render_target_name);

        error InstallShaderManagers();
        error ShutdownShaderManagers();

        error InstallRenderBufferManagers();
        error ShutdownRenderBufferManagers();

        error InstallTextureManagers();
        error ShutdownTextureManagers();

    private:
        static GraphicMain* m_instance;

        GraphicCoordSys m_coordSys;

        Frameworks::ServiceManager* m_serviceManager;

        std::unique_ptr<InstallingPolicy> m_policy;

        Renderer::RendererManager* m_renderer;
    };

};


#endif // GRAPHIC_MAIN_H
