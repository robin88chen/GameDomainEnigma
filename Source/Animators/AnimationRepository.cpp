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
#include "ModelAnimatorBuilder.h"
#include "AnimatorCommands.h"
#include <cassert>

#include "AnimationAssetDtos.h"

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
