#include "TerrainPrimitiveBuilder.h"
#include "Frameworks/ruid.h"
#include "Frameworks/RequestBus.h"
#include "TerrainPrimitiveRequests.h"
#include "Renderer/RendererErrors.h"

using namespace Enigma::Terrain;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;

DEFINE_RTTI(Terrain, TerrainPrimitiveBuilder, ISystemService);

TerrainPrimitiveBuilder::TerrainPrimitiveBuilder(Frameworks::ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
}

TerrainPrimitiveBuilder::~TerrainPrimitiveBuilder()
{
    std::lock_guard locker{ m_policiesLock };
    while (!m_policies.empty())
    {
        m_policies.pop();
    }
}

ServiceResult TerrainPrimitiveBuilder::OnInit()
{
    m_doBuildingPrimitive = std::make_shared<RequestSubscriber>([=](auto r) { this->DoBuildingPrimitive(r); });
    RequestBus::Subscribe(typeid(RequestBuildTerrainPrimitive), m_doBuildingPrimitive);

    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult TerrainPrimitiveBuilder::OnTick()
{
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult TerrainPrimitiveBuilder::OnTerm()
{
    RequestBus::Unsubscribe(typeid(RequestBuildTerrainPrimitive), m_doBuildingPrimitive);
    m_doBuildingPrimitive = nullptr;
    return Frameworks::ServiceResult::Complete;
}

error TerrainPrimitiveBuilder::BuildPrimitive(const Frameworks::Ruid& requester_ruid, const std::shared_ptr<TerrainPrimitivePolicy>& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push({ requester_ruid, policy });
    m_needTick = true;
    return ErrorCode::ok;
}

void TerrainPrimitiveBuilder::BuildTerrainPrimitive(const Frameworks::Ruid& requester_ruid, const std::shared_ptr<TerrainPrimitivePolicy>& policy)
{

}

void TerrainPrimitiveBuilder::DoBuildingPrimitive(const IRequestPtr& r)
{
    if (!r) return;
    const auto req = std::dynamic_pointer_cast<RequestBuildTerrainPrimitive, IRequest>(r);
    if (!req) return;
    BuildPrimitive(req->GetRuid(), req->GetPolicy());
}
