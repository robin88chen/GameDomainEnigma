#include "AnimationRepository.h"
#include "AnimatorErrors.h"
#include "Frameworks/CommandBus.h"
#include "AnimationAssetCommand.h"
#include "AnimationAssetBuilder.h"
#include "GameEngine/FactoryCommands.h"
#include "ModelAnimationAsset.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "AnimationAssetEvents.h"
#include "AnimatorEvents.h"
#include "ModelAnimatorBuilder.h"
#include "AnimatorCommands.h"
#include "Frameworks/StringFormat.h"
#include "AnimationAssetDtos.h"
#include "AnimationAssetPolicies.h"
#include "AnimatorPolicies.h"
#include <cassert>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Animators, AnimationRepository, ISystemService);

AnimationRepository::AnimationRepository(ServiceManager* manager) : ISystemService(manager)
{
    m_needTick = false;
    m_isAnimatorCurrentBuilding = false;
    m_isAssetCurrentBuilding = false;
    m_assetBuilder = std::make_unique<AnimationAssetBuilder>(this);
    m_animatorBuilder = std::make_unique<ModelAnimatorBuilder>(this);

    CommandBus::Post(std::make_shared<RegisterDtoFactory>(ModelAnimationAsset::TYPE_RTTI.GetName(),
        [=](auto o) { this->ModelAnimationAssetFactory(o); }));
}

AnimationRepository::~AnimationRepository()
{
    CommandBus::Post(std::make_shared<UnRegisterDtoFactory>(ModelAnimationAsset::TYPE_RTTI.GetName()));
    m_assetBuilder = nullptr;
    m_animatorBuilder = nullptr;
}

ServiceResult AnimationRepository::OnInit()
{
    m_doBuildingAnimationAsset = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingAnimationAsset(c); });
    m_doBuildingModelAnimator = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingModelAnimator(c); });
    CommandBus::Subscribe(typeid(Animators::BuildAnimationAsset), m_doBuildingAnimationAsset);
    CommandBus::Subscribe(typeid(Animators::BuildModelAnimator), m_doBuildingModelAnimator);

    m_onAnimationAssetBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnAnimationAssetBuilt(e); });
    m_onBuildAnimationAssetFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildAnimationAssetFailed(e); });
    m_onAnimatorBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnAnimatorBuilt(e); });
    m_onBuildAnimatorFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildAnimatorFailed(e); });
    EventPublisher::Subscribe(typeid(Animators::AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::Subscribe(typeid(Animators::BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);
    EventPublisher::Subscribe(typeid(Animators::ModelAnimatorBuilt), m_onAnimatorBuilt);
    EventPublisher::Subscribe(typeid(Animators::BuildModelAnimatorFailed), m_onBuildAnimatorFailed);

    return ServiceResult::Complete;
}

ServiceResult AnimationRepository::OnTick()
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
    m_buildingAssetName = m_assetPolicies.front()->Name();
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
    m_constructingAnimatorRuid = m_animatorPolicies.front()->GetRuid();
    m_animatorPolicies.pop();
    m_isAnimatorCurrentBuilding = true;
    return true;
}

ServiceResult AnimationRepository::OnTerm()
{
    CommandBus::Unsubscribe(typeid(Animators::BuildAnimationAsset), m_doBuildingAnimationAsset);
    CommandBus::Unsubscribe(typeid(Animators::BuildModelAnimator), m_doBuildingModelAnimator);
    m_doBuildingAnimationAsset = nullptr;
    m_doBuildingModelAnimator = nullptr;

    EventPublisher::Unsubscribe(typeid(Animators::AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::Unsubscribe(typeid(Animators::BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);
    EventPublisher::Unsubscribe(typeid(Animators::ModelAnimatorBuilt), m_onAnimatorBuilt);
    EventPublisher::Unsubscribe(typeid(Animators::BuildModelAnimatorFailed), m_onBuildAnimatorFailed);
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

std::shared_ptr<AnimationAsset> AnimationRepository::QueryAnimationAsset(const std::string& rtti_name, const std::string& name)
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
    if (dto.GetRtti().GetRttiName() != ModelAnimationAsset::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for geometry factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    auto animation = std::make_shared<ModelAnimationAsset>(ModelAnimationAssetDto::FromGenericDto(dto));
    EventPublisher::Post(std::make_shared<FactoryAnimationAssetCreated>(dto.GetId(), animation));
}

std::string AnimationRepository::MakeAssetKey(const std::string& rtti_name, const std::string& name)
{
    return name + rtti_name;
}

void AnimationRepository::DoBuildingAnimationAsset(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Animators::BuildAnimationAsset, ICommand>(c);
    if (!cmd) return;
    BuildAnimationAsset(cmd->GetPolicy());
}

void AnimationRepository::DoBuildingModelAnimator(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Animators::BuildModelAnimator, ICommand>(c);
    if (!cmd) return;
    BuildModelAnimator(cmd->GetPolicy());
}

void AnimationRepository::OnAnimationAssetBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Animators::AnimationAssetBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_buildingAssetName) return;
    m_isAssetCurrentBuilding = false;
}

void AnimationRepository::OnBuildAnimationAssetFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Animators::BuildAnimationAssetFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_buildingAssetName) return;
    Platforms::Debug::ErrorPrintf("animation asset %s build failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    m_isAssetCurrentBuilding = false;
}

void AnimationRepository::OnAnimatorBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    Ruid ruid_in_event;
    auto ev_model = std::dynamic_pointer_cast<Animators::ModelAnimatorBuilt, IEvent>(e);
    if (ev_model)
    {
        ruid_in_event = ev_model->GetRuid();
    }
    if (ruid_in_event != m_constructingAnimatorRuid) return;
    m_isAnimatorCurrentBuilding = false;
}

void AnimationRepository::OnBuildAnimatorFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    Ruid ruid_in_event;
    std::string err_msg;
    auto ev_model = std::dynamic_pointer_cast<Animators::BuildModelAnimatorFailed, IEvent>(e);
    if (ev_model)
    {
        ruid_in_event = ev_model->GetRuid();
        err_msg = string_format("model animator build failed : %s\n", ev_model->GetErrorCode().message().c_str());
    }
    if (ruid_in_event != m_constructingAnimatorRuid) return;
    Platforms::Debug::ErrorPrintf(err_msg.c_str());
    m_isAnimatorCurrentBuilding = false;
}
