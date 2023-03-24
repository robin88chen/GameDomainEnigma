#include "AnimatorInstallingPolicy.h"
#include "AnimationRepository.h"
#include "AnimationFrameListener.h"
#include "GameEngine/TimerService.h"
#include <cassert>

using namespace Enigma::Animators;

void AnimatorInstallingPolicy::Install(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->RegisterSystemService(std::make_shared<AnimationRepository>(service_manager));
    auto timer = service_manager->GetSystemServiceAs<Engine::TimerService>();
    service_manager->RegisterSystemService(std::make_shared<AnimationFrameListener>(service_manager, timer));
}

void AnimatorInstallingPolicy::Shutdown(Frameworks::ServiceManager* service_manager)
{
    assert(service_manager);
    service_manager->ShutdownSystemService(AnimationFrameListener::TYPE_RTTI);
    service_manager->ShutdownSystemService(AnimationRepository::TYPE_RTTI);
}

