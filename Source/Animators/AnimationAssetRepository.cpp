#include "AnimationAssetRepository.h"
#include "AnimatorErrors.h"
#include "Frameworks/CommandBus.h"
#include "AnimationAssetCommand.h"
#include "AnimationAssetBuilder.h"
#include "GameEngine/FactoryCommands.h"
#include "ModelAnimationAsset.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "AnimationAssetEvents.h"
#include <cassert>

#include "AnimationAssetDtos.h"

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Animators, AnimationAssetRepository, ISystemService);

AnimationAssetRepository::AnimationAssetRepository(ServiceManager* manager) : ISystemService(manager)
{
    m_needTick = false;
    m_builder = std::make_unique<AnimationAssetBuilder>(this);
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(ModelAnimationAsset::TYPE_RTTI.GetName(),
        [=](auto o) { this->ModelAnimationAssetFactory(o); }));
}

AnimationAssetRepository::~AnimationAssetRepository()
{
    CommandBus::Post(std::make_shared<UnRegisterDtoFactory>(ModelAnimationAsset::TYPE_RTTI.GetName()));
    m_builder = nullptr;
}

ServiceResult AnimationAssetRepository::OnInit()
{
    m_doBuildingAnimationAsset = std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingAnimationAsset(c); });
    CommandBus::Subscribe(typeid(Animators::BuildAnimationAsset), m_doBuildingAnimationAsset);

    return ServiceResult::Complete;
}

ServiceResult AnimationAssetRepository::OnTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_builder);
    m_builder->BuildAnimationAsset(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return ServiceResult::Pendding;
}

ServiceResult AnimationAssetRepository::OnTerm()
{
    CommandBus::Unsubscribe(typeid(Animators::BuildAnimationAsset), m_doBuildingAnimationAsset);
    m_doBuildingAnimationAsset = nullptr;

    return ServiceResult::Complete;
}

bool AnimationAssetRepository::HasAnimationAsset(const std::string& rtti_name, const std::string& name)
{
    std::lock_guard locker{ m_assetsLock };
    auto it = m_assets.find(MakeAssetKey(rtti_name, name));
    return ((it != m_assets.end()) && (!it->second.expired()));
}

std::shared_ptr<AnimationAsset> AnimationAssetRepository::QueryAnimationAsset(const std::string& rtti_name, const std::string& name)
{
    std::lock_guard locker{ m_assetsLock };
    auto it = m_assets.find(MakeAssetKey(rtti_name, name));
    if (it == m_assets.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

error AnimationAssetRepository::BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

void AnimationAssetRepository::ModelAnimationAssetFactory(const Engine::GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != ModelAnimationAsset::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for geometry factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    auto animation = std::make_shared<ModelAnimationAsset>(ModelAnimationAssetDto::FromGenericDto(dto));
    EventPublisher::Post(std::make_shared<FactoryAnimationAssetCreated>(dto.GetId(), animation));
}

std::string AnimationAssetRepository::MakeAssetKey(const std::string& rtti_name, const std::string& name)
{
    return name + rtti_name;
}

void AnimationAssetRepository::DoBuildingAnimationAsset(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Animators::BuildAnimationAsset, ICommand>(c);
    if (!cmd) return;
    BuildAnimationAsset(cmd->GetPolicy());
}
