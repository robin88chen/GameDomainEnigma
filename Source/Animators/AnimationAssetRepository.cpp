#include "AnimationAssetRepository.h"
#include "AnimatorErrors.h"
#include <cassert>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Animators, AnimationAssetRepository, ISystemService);

AnimationAssetRepository::AnimationAssetRepository(ServiceManager* manager) : ISystemService(manager)
{
    m_needTick = false;
}

AnimationAssetRepository::~AnimationAssetRepository()
{
}

ServiceResult AnimationAssetRepository::OnInit()
{
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
    //assert(m_builder);
    //m_builder->BuildGeometry(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return ServiceResult::Pendding;
}

ServiceResult AnimationAssetRepository::OnTerm()
{
    return ServiceResult::Complete;
}

bool AnimationAssetRepository::HasAnimationAsset(const Rtti& type_rtti, const std::string& name)
{
    std::lock_guard locker{ m_assetsLock };
    auto it = m_assets.find(MakeAssetKey(type_rtti, name));
    return ((it != m_assets.end()) && (!it->second.expired()));
}

std::shared_ptr<AnimationAsset> AnimationAssetRepository::QueryAnimationAsset(const Rtti& type_rtti, const std::string& name)
{
    std::lock_guard locker{ m_assetsLock };
    auto it = m_assets.find(MakeAssetKey(type_rtti, name));
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

std::string AnimationAssetRepository::MakeAssetKey(const Rtti& type_rtti, const std::string& name)
{
    return name + type_rtti.GetName();
}
