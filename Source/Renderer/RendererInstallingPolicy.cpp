#include "RendererInstallingPolicy.h"
#include "RendererManager.h"
#include "RenderTarget.h"
#include "RendererErrors.h"
#include <cassert>

using namespace Enigma::Renderer;

error RenderSystemInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->registerSystemService(std::make_shared<RendererManager>(service_manager));
    return ErrorCode::ok;
}

error RenderSystemInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(RendererManager::TYPE_RTTI);
    return ErrorCode::ok;
}

error DefaultRendererInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    const auto manager = service_manager->getSystemServiceAs<RendererManager>();
    assert(manager);
    error er = manager->createRenderer(m_rendererName);
    if (er) return er;
    er = manager->createRenderTarget(m_primaryTargetName, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
    return er;
}

error DefaultRendererInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    const auto manager = service_manager->getSystemServiceAs<RendererManager>();
    assert(manager);
    error er = manager->destroyRenderer(m_rendererName);
    if (er) return er;
    er = manager->destroyRenderTarget(m_primaryTargetName);
    return er;
}
