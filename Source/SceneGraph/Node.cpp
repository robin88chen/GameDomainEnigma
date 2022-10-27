#include "Node.h"
#include "Culler.h"
#include "SceneGraphErrors.h"
#include "SceneGraphEvents.h"
#include "SceneGraphContracts.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/LinkageResolver.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;

Enigma::Frameworks::Rtti Node::TYPE_RTTI = Enigma::Frameworks::Rtti("Enigma.SceneGraph.Node", Spatial::TYPE_RTTI);

Node::Node(const std::string& name) : Spatial(name)
{
}

Node::Node(const NodeContract& contract, Engine::ContractedLinkageResolver<Spatial>& resolver)
    : Spatial(dynamic_cast<const SpatialContract&>(contract))
{
    for (auto& name : contract.ChildNames())
    {
        resolver.TryResolveLinkage(name, [=](auto sp) { this->AttachChild(sp, sp->GetLocalTransform()); });
    }
}

Node::~Node()
{
    while (m_childList.size())
    {
        SpatialPtr child = m_childList.front();
        m_childList.pop_front();
        if (child) child->LinkParent(nullptr);
        child = nullptr;
    }
    m_childList.clear();
}

error Node::OnCullingVisible(Culler* culler, bool noCull)
{
    culler->Insert(ThisSpatial());

    if (m_childList.size() == 0) return ErrorCode::ok;
    error er = ErrorCode::ok;
    ChildList::iterator iter = m_childList.begin();
    while (iter != m_childList.end())
    {
        er = (*iter)->CullVisibleSet(culler, noCull);
        if (er) return er;

        ++iter;
    }
    return ErrorCode::ok;
}

SceneTraveler::TravelResult Node::VisitBy(SceneTraveler* traveler)
{
    if (!traveler) return SceneTraveler::TravelResult::InterruptError;

    SceneTraveler::TravelResult res = traveler->TravelTo(ThisSpatial());
    if (res != SceneTraveler::TravelResult::Continue) return res;  // don't go sub-tree

    if (m_childList.size() == 0) return res;

    ChildList::iterator iter = m_childList.begin();
    while (iter != m_childList.end())
    {
        res = (*iter)->VisitBy(traveler);
        if (res == SceneTraveler::TravelResult::InterruptError) return res;
        if (res == SceneTraveler::TravelResult::InterruptTargetFound) return res;

        ++iter;
    }
    return res;
}

error Node::AttachChild(const SpatialPtr& child, const MathLib::Matrix4& mxChildLocal)
{
    if (FATAL_LOG_EXPR(!child)) return ErrorCode::nullSceneGraph;
    if (FATAL_LOG_EXPR(child->GetParent() != nullptr)) return ErrorCode::parentNode; // must not have parent, must detach first!!

    m_childList.push_back(child);
    child->LinkParent(ThisSpatial());

    Frameworks::EventPublisher::Post(std::make_shared<SceneGraphChanged>(ThisSpatial(), child, SceneGraphChanged::NotifyCode::AttachChild));

    error er = child->SetLocalTransform(mxChildLocal);

    child->_UpdateSpatialRenderState();

    return er;
}

error Node::DetachChild(const SpatialPtr& child)
{
    if (FATAL_LOG_EXPR(!child)) return ErrorCode::nullSceneGraph;
    if (FATAL_LOG_EXPR(child->GetParent() != ThisSpatial())) return ErrorCode::parentNode;

    child->LinkParent(nullptr);

    Frameworks::EventPublisher::Post(std::make_shared<SceneGraphChanged>(ThisSpatial(), child, SceneGraphChanged::NotifyCode::DetachChild));

    error er = child->SetLocalTransform(MathLib::Matrix4::IDENTITY);

    child->_UpdateSpatialRenderState();

    // now, remove child, if child has no more reference, it will be deleted
    m_childList.remove(child);

    return er;
}

error Node::_UpdateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    m_mxLocalTransform = mxLocal;

    MathLib::Matrix4 mxParent = MathLib::Matrix4::IDENTITY;
    if (GetParent())
    {
        mxParent = GetParent()->GetWorldTransform();
    }
    error er = _UpdateWorldData(mxParent);
    if (er) return er;

    if (TestNotifyFlag(Notify_Location))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialLocationChanged>(ThisSpatial()));
    }

    // propagate up
    er = _UpdateBoundData();

    return er;
}

error Node::_UpdateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_UpdateWorldData(mxParentWorld);
    if (er) return er;

    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            er = (*iter)->_UpdateWorldData(m_mxWorldTransform);
            if (er) return er;

            ++iter;
        }
    }
    return er;
}

error Node::_UpdateBoundData()
{
    m_modelBound = Engine::BoundingVolume();
    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            if (m_modelBound.IsEmpty())
            {
                m_modelBound = Engine::BoundingVolume::CreateFromTransform((*iter)->GetModelBound(), (*iter)->GetLocalTransform());
            }
            else
            {
                m_modelBound.Merge((*iter)->GetLocalTransform(), (*iter)->GetModelBound());
            }
            ++iter;
        }
    }
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);

    if (TestNotifyFlag(Notify_Bounding))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialBoundChanged>(ThisSpatial()));
    }

    error er = ErrorCode::ok;
    if (GetParent())
    {
        er = GetParent()->_UpdateBoundData();
    }
    return er;
}

error Node::_UpdateSpatialRenderState()
{
    Spatial::_UpdateSpatialRenderState();

    error er = ErrorCode::ok;
    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            er = (*iter)->_UpdateSpatialRenderState();
            if (er) return er;

            ++iter;
        }
    }
    if (TestNotifyFlag(Notify_RenderState))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialRenderStateChanged>(ThisSpatial()));
    }
    return er;
}

error Node::_PropagateSpatialRenderState()
{
    error er = ErrorCode::ok;

    if (GetParent())
    {
        er = GetParent()->_PropagateSpatialRenderState();
    }
    else
    {
        er = _UpdateSpatialRenderState();
    }

    return er;
}
