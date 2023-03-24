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
#include "InstallingPolicies.h"

namespace Enigma::Renderer
{
    class RendererManager;
};

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

        error InstallRenderEngine(const std::vector<std::shared_ptr<InstallingPolicy>>& policies);
        error ShutdownRenderEngine();

        /** frame update (service manager call run one to update) */
        void FrameUpdate();

        Frameworks::ServiceManager* GetServiceManager() { return m_serviceManager; };

    private:
        error CreateRenderEngineDevice(const std::shared_ptr<DeviceCreatingPolicy>& policy);
        error CleanupRenderEngineDevice();

        error InstallRenderer(const std::shared_ptr<RendererInstallingPolicy>& policy);
        error ShutdownRenderer(const std::shared_ptr<RendererInstallingPolicy>& policy);

        //error InstallRenderer(const std::string& renderer_name, const std::string render_target_name, bool is_primary);
        //error ShutdownRenderer(const std::string& renderer_name, const std::string render_target_name);

        error InstallRenderManagers();
        error ShutdownRenderManagers();

        error InstallGeometryManagers();
        error ShutdownGeometryManagers();

        error InstallShaderManagers();
        error ShutdownShaderManagers();

        error InstallRenderBufferManagers();
        error ShutdownRenderBufferManagers();

        error InstallTextureManagers();
        error ShutdownTextureManagers();

        error InstallSceneGraphManagers(const std::shared_ptr<SceneGraphBuildingPolicy>& policy);
        error ShutdownSceneGraphManagers();

        error InstallAnimationServices();
        error ShutdownAnimationServices();

        error InstallInputHandlers(const std::shared_ptr<InputHandlerInstallingPolicy>& policy);
        error ShutdownInputHandlers();

    private:
        static GraphicMain* m_instance;

        Frameworks::ServiceManager* m_serviceManager;

        InstallingPolicyGroup m_policies;

        std::shared_ptr<Renderer::RendererManager> m_renderer;
    };

};


#endif // GRAPHIC_MAIN_H
