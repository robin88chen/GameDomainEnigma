#include "RenderablesInstallingPolicy.h"
#include "RenderablePrimitiveBuilder.h"
#include "ModelPrimitiveRay3IntersectionFinder.h"
#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "Primitives/PrimitiveIntersectionFinderFactories.h"
#include "MeshPrimitive.h"
#include "ModelPrimitive.h"
#include "ModelPrimitiveAnimator.h"
#include "ModelAnimationAsset.h"
#include "Frameworks/CommandBus.h"
#include "Animators/AnimatorRepository.h"
#include "Animators/AnimatorFactory.h"
#include "Animators/AnimationAssetRepository.h"
#include "Animators/AnimationAssetFactory.h"
#include <system_error>

using namespace Enigma::Renderables;

error RenderablesInstallingPolicy::install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    auto geometry_repository = service_manager->getSystemServiceAs<Geometries::GeometryRepository>();
    assert(geometry_repository);
    auto primitive_repository = service_manager->getSystemServiceAs<Primitives::PrimitiveRepository>();
    assert(primitive_repository);
    service_manager->registerSystemService(std::make_shared<RenderablePrimitiveBuilder>(service_manager, primitive_repository, geometry_repository));

    auto animator_repository = service_manager->getSystemServiceAs<Animators::AnimatorRepository>();
    assert(animator_repository);
    animator_repository->factory()->registerAnimatorFactory(ModelPrimitiveAnimator::TYPE_RTTI.getName(), ModelPrimitiveAnimator::create, ModelPrimitiveAnimator::constitute);
    auto animation_asset_repository = service_manager->getSystemServiceAs<Animators::AnimationAssetRepository>();
    assert(animation_asset_repository);
    animation_asset_repository->factory()->registerAnimationAssetFactory(ModelAnimationAsset::TYPE_RTTI.getName(), ModelAnimationAsset::create, ModelAnimationAsset::constitute);

    Primitives::PrimitiveRay3IntersectionFinderFactory::registerCreator(ModelPrimitive::TYPE_RTTI.getName(), ModelPrimitiveRay3IntersectionFinder::create);
    Primitives::PrimitiveRay3IntersectionFinderFactory::registerCreator(MeshPrimitive::TYPE_RTTI.getName(), MeshPrimitiveRay3IntersectionFinder::create);
    return std::error_code();
}

error RenderablesInstallingPolicy::shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->shutdownSystemService(RenderablePrimitiveBuilder::TYPE_RTTI);
    return std::error_code();
}

