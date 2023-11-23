#include "Node.h"
#include "Culler.h"
#include "Spatial.h"
#include "SceneGraphErrors.h"
#include "SceneGraphEvents.h"
#include "SceneGraphDtos.h"
#include "SceneFlattenTraversal.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/LinkageResolver.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, Node, Spatial);

Node::Node(const std::string& name) : Spatial(name)
{
    m_factoryDesc = Engine::FactoryDesc(Node::TYPE_RTTI.getName());
}

Node::Node(const Engine::GenericDto& o) : Spatial(o)
{
}

Node::~Node()
{
    while (m_childList.size())
    {
        SpatialPtr child = m_childList.front();
        m_childList.pop_front();
        if (child) child->linkParent(nullptr);
        child = nullptr;
    }
    m_childList.clear();
}

Enigma::Engine::GenericDto Node::serializeDto()
{
    return serializeNodeDto().toGenericDto();
}

NodeDto Node::serializeNodeDto()
{
    NodeDto dto(serializeSpatialDto());
    for (auto child : m_childList)
    {
        if (child) dto.ChildNames().emplace_back(child->getSpatialName());
    }
    return dto;
}

void Node::resolveFactoryLinkage(const Engine::GenericDto& dto, Engine::FactoryLinkageResolver<Spatial>& resolver)
{
    NodeDto nodeDto = NodeDto::fromGenericDto(dto);
    for (auto& name : nodeDto.ChildNames())
    {
        resolver.TryResolveLinkage(name, [lifetime = weak_from_this()](auto sp)
            { if (!lifetime.expired())
            std::dynamic_pointer_cast<Node, Spatial>(lifetime.lock())->attachChild(sp, sp->getLocalTransform()); });
    }
}

Enigma::Engine::GenericDtoCollection Node::serializeFlattenedTree()
{
    Engine::GenericDtoCollection collection;
    SceneFlattenTraversal flatten;
    visitBy(&flatten);
    if (flatten.GetSpatials().empty()) return collection;
    for (auto& sp : flatten.GetSpatials())
    {
        collection.push_back(sp->serializeDto());
    }
    collection[0].AsTopLevel(true);
    return collection;
}

error Node::onCullingVisible(Culler* culler, bool noCull)
{
    culler->Insert(thisSpatial());

    if (m_childList.size() == 0) return ErrorCode::ok;
    error er = ErrorCode::ok;
    ChildList::iterator iter = m_childList.begin();
    while (iter != m_childList.end())
    {
        er = (*iter)->cullVisibleSet(culler, noCull);
        if (er) return er;

        ++iter;
    }
    return ErrorCode::ok;
}

SceneTraveler::TravelResult Node::visitBy(SceneTraveler* traveler)
{
    if (!traveler) return SceneTraveler::TravelResult::InterruptError;

    SceneTraveler::TravelResult res = traveler->TravelTo(thisSpatial());
    if (res != SceneTraveler::TravelResult::Continue) return res;  // don't go sub-tree

    if (m_childList.size() == 0) return res;

    ChildList::iterator iter = m_childList.begin();
    while (iter != m_childList.end())
    {
        res = (*iter)->visitBy(traveler);
        if (res == SceneTraveler::TravelResult::InterruptError) return res;
        if (res == SceneTraveler::TravelResult::InterruptTargetFound) return res;

        ++iter;
    }
    return res;
}

error Node::attachChild(const SpatialPtr& child, const MathLib::Matrix4& mxChildLocal)
{
    if (FATAL_LOG_EXPR(!child)) return ErrorCode::nullSceneGraph;
    if (FATAL_LOG_EXPR(child->getParent() != nullptr)) return ErrorCode::parentNode; // must not have parent, must detach first!!

    m_childList.push_back(child);
    child->linkParent(thisSpatial());

    Frameworks::EventPublisher::post(std::make_shared<SceneGraphChanged>(thisSpatial(), child, SceneGraphChanged::NotifyCode::AttachChild));

    error er = child->setLocalTransform(mxChildLocal);

    child->_updateSpatialRenderState();

    return er;
}

error Node::detachChild(const SpatialPtr& child)
{
    if (FATAL_LOG_EXPR(!child)) return ErrorCode::nullSceneGraph;
    if (FATAL_LOG_EXPR(child->getParent() != thisSpatial())) return ErrorCode::parentNode;

    child->linkParent(nullptr);

    Frameworks::EventPublisher::post(std::make_shared<SceneGraphChanged>(thisSpatial(), child, SceneGraphChanged::NotifyCode::DetachChild));

    error er = child->setLocalTransform(MathLib::Matrix4::IDENTITY);

    child->_updateSpatialRenderState();

    // now, remove child, if child has no more reference, it will be deleted
    m_childList.remove(child);

    return er;
}

error Node::_updateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    m_mxLocalTransform = mxLocal;

    MathLib::Matrix4 mxParent = MathLib::Matrix4::IDENTITY;
    if (getParent())
    {
        mxParent = getParent()->getWorldTransform();
    }
    error er = _updateWorldData(mxParent);
    if (er) return er;

    if (testNotifyFlag(Notify_Location))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialLocationChanged>(thisSpatial()));
    }

    // propagate up
    er = _updateBoundData();

    return er;
}

error Node::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_updateWorldData(mxParentWorld);
    if (er) return er;

    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            er = (*iter)->_updateWorldData(m_mxWorldTransform);
            if (er) return er;

            ++iter;
        }
    }
    return er;
}

error Node::_updateBoundData()
{
    m_modelBound = Engine::BoundingVolume();
    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            if (m_modelBound.IsEmpty())
            {
                m_modelBound = Engine::BoundingVolume::CreateFromTransform((*iter)->getModelBound(), (*iter)->getLocalTransform());
            }
            else
            {
                m_modelBound.Merge((*iter)->getLocalTransform(), (*iter)->getModelBound());
            }
            ++iter;
        }
    }
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);

    if (testNotifyFlag(Notify_Bounding))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialBoundChanged>(thisSpatial()));
    }

    error er = ErrorCode::ok;
    if (getParent())
    {
        er = getParent()->_updateBoundData();
    }
    return er;
}

error Node::_updateSpatialRenderState()
{
    Spatial::_updateSpatialRenderState();

    error er = ErrorCode::ok;
    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            er = (*iter)->_updateSpatialRenderState();
            if (er) return er;

            ++iter;
        }
    }
    if (testNotifyFlag(Notify_RenderState))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialRenderStateChanged>(thisSpatial()));
    }
    return er;
}

error Node::_propagateSpatialRenderState()
{
    error er;

    if (getParent())
    {
        er = getParent()->_propagateSpatialRenderState();
    }
    else
    {
        er = _updateSpatialRenderState();
    }

    return er;
}
