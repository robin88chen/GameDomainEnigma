#include "RendererInstallingPolicy.h"
#include "RendererManager.h"
#include "RenderablePrimitiveBuilder.h"
#include "RenderTarget.h"
#include "RendererErrors.h"
#include "ModelPrimitiveRay3IntersectionFinder.h"
#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "Primitives/PrimitiveIntersectionFinderFactories.h"
#include "MeshPrimitive.h"
#include "ModelPrimitive.h"
#include <cassert>

using namespace Enigma::Renderer;

error RenderSystemInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto geometry_repository = service_manager->getSystemServiceAs<Geometries::GeometryRepository>();
    assert(geometry_repository);
    auto primitive_repository = service_manager->getSystemServiceAs<Primitives::PrimitiveRepository>();
    assert(primitive_repository);
    service_manager->registerSystemService(std::make_shared<RendererManager>(service_manager));
    service_manager->registerSystemService(std::make_shared<RenderablePrimitiveBuilder>(service_manager, primitive_repository, geometry_repository));

    Primitives::PrimitiveRay3IntersectionFinderFactory::RegisterCreator(ModelPrimitive::TYPE_RTTI.getName(), ModelPrimitiveRay3IntersectionFinder::create);
    Primitives::PrimitiveRay3IntersectionFinderFactory::RegisterCreator(MeshPrimitive::TYPE_RTTI.getName(), MeshPrimitiveRay3IntersectionFinder::create);
    return ErrorCode::ok;
}

error RenderSystemInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(RenderablePrimitiveBuilder::TYPE_RTTI);
    service_manager->shutdownSystemService(RendererManager::TYPE_RTTI);
    return ErrorCode::ok;
}

error DefaultRendererInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    const auto manager = service_manager->getSystemServiceAs<RendererManager>();
    assert(manager);
    error er = manager->CreateRenderer(m_rendererName);
    if (er) return er;
    er = manager->CreateRenderTarget(m_primaryTargetName, RenderTarget::PrimaryType::IsPrimary, { Graphics::RenderTextureUsage::Default });
    return er;
}

error DefaultRendererInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    const auto manager = service_manager->getSystemServiceAs<RendererManager>();
    assert(manager);
    error er = manager->DestroyRenderer(m_rendererName);
    if (er) return er;
    er = manager->DestroyRenderTarget(m_primaryTargetName);
    return er;
}
