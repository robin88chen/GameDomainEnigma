#include "LazyNodeIOService.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraphCommands.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

DEFINE_RTTI(SceneGraph, LazyNodeIOService, ISystemService);

LazyNodeIOService::LazyNodeIOService(Frameworks::ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
    m_doInstancingLazyNode = std::make_shared<CommandSubscriber>([=](auto c) { DoInstancingLazyNode(c); });
    CommandBus::Subscribe(typeid(InstanceLazyNode), m_doInstancingLazyNode);
}

LazyNodeIOService::~LazyNodeIOService()
{
    CommandBus::Unsubscribe(typeid(InstanceLazyNode), m_doInstancingLazyNode);
    m_doInstancingLazyNode = nullptr;
}

void LazyNodeIOService::DoInstancingLazyNode(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InstanceLazyNode, ICommand>(c);
    if (!cmd) return;
    if (!cmd->GetNode()) return;

}
