#include "RendererInstallingPolicy.h"
#include "RendererManager.h"
#include "RenderablePrimitiveBuilder.h"
#include "RenderTarget.h"
#include "RendererErrors.h"
#include "ModelPrimitiveRay3IntersectionFinder.h"
#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "GameEngine/IntersectionFinderFactories.h"
#include "MeshPrimitive.h"
#include "ModelPrimitive.h"
#include <cassert>

using namespace Enigma::Renderer;

error RenderSystemInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->RegisterSystemService(std::make_shared<RendererManager>(service_manager));
    service_manager->RegisterSystemService(std::make_shared<RenderablePrimitiveBuilder>(service_manager));

    Engine::PrimitiveRay3IntersectionFinderFactory::RegisterCreator(ModelPrimitive::TYPE_RTTI.getName(), ModelPrimitiveRay3IntersectionFinder::Create);
    Engine::PrimitiveRay3IntersectionFinderFactory::RegisterCreator(MeshPrimitive::TYPE_RTTI.getName(), MeshPrimitiveRay3IntersectionFinder::Create);
    return ErrorCode::ok;
}

error RenderSystemInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(RenderablePrimitiveBuilder::TYPE_RTTI);
    service_manager->ShutdownSystemService(RendererManager::TYPE_RTTI);
    return ErrorCode::ok;
}

error DefaultRendererInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    const auto manager = service_manager->GetSystemServiceAs<RendererManager>();
    assert(manager);
    error er = manager->CreateRenderer(m_rendererName);
    if (er) return er;
    er = manager->CreateRenderTarget(m_primaryTargetName, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
    return er;
}

error DefaultRendererInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    const auto manager = service_manager->GetSystemServiceAs<RendererManager>();
    assert(manager);
    error er = manager->DestroyRenderer(m_rendererName);
    if (er) return er;
    er = manager->DestroyRenderTarget(m_primaryTargetName);
    return er;
}
