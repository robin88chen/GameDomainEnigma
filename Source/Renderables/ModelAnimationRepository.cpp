#include "ModelAnimationRepository.h"
#include "RenderableErrors.h"
#include "Frameworks/CommandBus.h"
#include "RenderableCommands.h"
#include "AnimationAssetBuilder.h"
#include "GameEngine/FactoryCommands.h"
#include "ModelAnimationAsset.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "RenderableEvents.h"
#include "ModelAnimatorBuilder.h"
#include "Frameworks/StringFormat.h"
#include "ModelAnimationDtos.h"
#include "AnimationAssetPolicies.h"
#include "AnimatorPolicies.h"
#include <cassert>

using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderables, AnimationRepository, ISystemService);

AnimationRepository::AnimationRepository(ServiceManager* manager) : ISystemService(manager), m_constructingAnimatorRuid()
{
    m_needTick = false;
    m_isAnimatorCurrentBuilding = false;
    m_isAssetCurrentBuilding = false;
    m_assetBuilder = std::make_unique<AnimationAssetBuilder>(this);
    m_animatorBuilder = std::make_unique<ModelAnimatorBuilder>(this);

    CommandBus::post(std::make_shared<RegisterDtoFactory>(ModelAnimationAsset::TYPE_RTTI.getName(),
        [=](auto o) { this->ModelAnimationAssetFactory(o); }));
}

AnimationRepository::~AnimationRepository()
{
    CommandBus::post(std::make_shared<UnRegisterDtoFactory>(ModelAnimationAsset::TYPE_RTTI.getName()));
    m_assetBuilder = nullptr;
    m_animatorBuilder = nullptr;
}

ServiceResult AnimationRepository::onInit()
{
    m_doBuildingAnimationAsset = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingAnimationAsset(c); });
    m_doBuildingModelAnimator = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingModelAnimator(c); });
    CommandBus::subscribe(typeid(Renderables::BuildAnimationAsset), m_doBuildingAnimationAsset);
    CommandBus::subscribe(typeid(Renderables::BuildModelAnimator), m_doBuildingModelAnimator);

    m_onAnimationAssetBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnAnimationAssetBuilt(e); });
    m_onBuildAnimationAssetFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildAnimationAssetFailed(e); });
    m_onAnimatorBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnAnimatorBuilt(e); });
    m_onBuildAnimatorFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildAnimatorFailed(e); });
    EventPublisher::subscribe(typeid(AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::subscribe(typeid(BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);
    EventPublisher::subscribe(typeid(ModelAnimatorBuilt), m_onAnimatorBuilt);
    EventPublisher::subscribe(typeid(BuildModelAnimatorFailed), m_onBuildAnimatorFailed);

    return ServiceResult::Complete;
}

ServiceResult AnimationRepository::onTick()
{
    if ((m_isAssetCurrentBuilding) && (m_isAnimatorCurrentBuilding)) return Frameworks::ServiceResult::Pendding;  // 都在處理中
    bool is_building = PopAssetBuilding();
    is_building |= PopAnimatorBuilding();
    if (!is_building)
    {
        m_needTick = false;
    }
    return ServiceResult::Pendding;
}

bool AnimationRepository::PopAssetBuilding()
{
    if (m_isAssetCurrentBuilding) return true;
    std::lock_guard locker{ m_assetPoliciesLock };
    if (m_assetPolicies.empty()) return false;
    assert(m_assetBuilder);
    m_assetBuilder->BuildAnimationAsset(m_assetPolicies.front());
    m_buildingAssetName = m_assetPolicies.front()->name();
    m_assetPolicies.pop();
    m_isAssetCurrentBuilding = true;
    return true;
}

bool AnimationRepository::PopAnimatorBuilding()
{
    if (m_isAnimatorCurrentBuilding) return true;
    std::lock_guard locker{ m_animatorPoliciesLock };
    if (m_animatorPolicies.empty()) return false;
    assert(m_animatorBuilder);
    m_animatorBuilder->BuildModelAnimator(m_animatorPolicies.front());
    m_constructingAnimatorRuid = m_animatorPolicies.front()->getRuid();
    m_animatorPolicies.pop();
    m_isAnimatorCurrentBuilding = true;
    return true;
}

ServiceResult AnimationRepository::onTerm()
{
    CommandBus::unsubscribe(typeid(Renderables::BuildAnimationAsset), m_doBuildingAnimationAsset);
    CommandBus::unsubscribe(typeid(Renderables::BuildModelAnimator), m_doBuildingModelAnimator);
    m_doBuildingAnimationAsset = nullptr;
    m_doBuildingModelAnimator = nullptr;

    EventPublisher::unsubscribe(typeid(AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::unsubscribe(typeid(BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);
    EventPublisher::unsubscribe(typeid(ModelAnimatorBuilt), m_onAnimatorBuilt);
    EventPublisher::unsubscribe(typeid(BuildModelAnimatorFailed), m_onBuildAnimatorFailed);
    m_onAnimationAssetBuilt = nullptr;
    m_onBuildAnimationAssetFailed = nullptr;
    m_onAnimatorBuilt = nullptr;
    m_onBuildAnimatorFailed = nullptr;

    return ServiceResult::Complete;
}

bool AnimationRepository::HasAnimationAsset(const std::string& rtti_name, const std::string& name)
{
    std::lock_guard locker{ m_assetsLock };
    auto it = m_assets.find(MakeAssetKey(rtti_name, name));
    return ((it != m_assets.end()) && (!it->second.expired()));
}

std::shared_ptr<Enigma::Animators::AnimationAsset> AnimationRepository::QueryAnimationAsset(const std::string& rtti_name, const std::string& name)
{
    std::lock_guard locker{ m_assetsLock };
    auto it = m_assets.find(MakeAssetKey(rtti_name, name));
    if (it == m_assets.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

error AnimationRepository::BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy)
{
    std::lock_guard locker{ m_assetPoliciesLock };
    m_assetPolicies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

error AnimationRepository::BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy)
{
    std::lock_guard locker{ m_animatorPoliciesLock };
    m_animatorPolicies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void AnimationRepository::ModelAnimationAssetFactory(const Engine::GenericDto& dto)
{
    if (dto.getRtti().GetRttiName() != ModelAnimationAsset::TYPE_RTTI.getName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for geometry factory", dto.getRtti().GetRttiName().c_str());
        return;
    }
    //auto animation = std::make_shared<ModelAnimationAsset>(ModelAnimationAssetDto::fromGenericDto(dto));
    //EventPublisher::post(std::make_shared<FactoryAnimationAssetCreated>(dto.ruid(), animation));
}

std::string AnimationRepository::MakeAssetKey(const std::string& rtti_name, const std::string& name)
{
    return name + rtti_name;
}

void AnimationRepository::DoBuildingAnimationAsset(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Renderables::BuildAnimationAsset, ICommand>(c);
    if (!cmd) return;
    BuildAnimationAsset(cmd->GetPolicy());
}

void AnimationRepository::DoBuildingModelAnimator(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Renderables::BuildModelAnimator, ICommand>(c);
    if (!cmd) return;
    BuildModelAnimator(cmd->GetPolicy());
}

void AnimationRepository::OnAnimationAssetBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<AnimationAssetBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_buildingAssetName) return;
    m_isAssetCurrentBuilding = false;
}

void AnimationRepository::OnBuildAnimationAssetFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildAnimationAssetFailed, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_buildingAssetName) return;
    Platforms::Debug::ErrorPrintf("animation asset %s build failed : %s\n",
        ev->getName().c_str(), ev->GetErrorCode().message().c_str());
    m_isAssetCurrentBuilding = false;
}

void AnimationRepository::OnAnimatorBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    Ruid ruid_in_event{};
    auto ev_model = std::dynamic_pointer_cast<ModelAnimatorBuilt, IEvent>(e);
    if (ev_model)
    {
        ruid_in_event = ev_model->getRuid();
    }
    if (ruid_in_event != m_constructingAnimatorRuid) return;
    m_isAnimatorCurrentBuilding = false;
}

void AnimationRepository::OnBuildAnimatorFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    Ruid ruid_in_event{};
    std::string err_msg;
    auto ev_model = std::dynamic_pointer_cast<BuildModelAnimatorFailed, IEvent>(e);
    if (ev_model)
    {
        ruid_in_event = ev_model->getRuid();
        err_msg = string_format("model animator build failed : %s\n", ev_model->GetErrorCode().message().c_str());
    }
    if (ruid_in_event != m_constructingAnimatorRuid) return;
    Platforms::Debug::ErrorPrintf(err_msg.c_str());
    m_isAnimatorCurrentBuilding = false;
}
