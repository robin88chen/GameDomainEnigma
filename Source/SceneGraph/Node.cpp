#include "Node.h"
#include "NodeAssembler.h"
#include "Culler.h"
#include "Spatial.h"
#include "SceneGraphErrors.h"
#include "SceneGraphEvents.h"
#include "SceneFlattenTraversal.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/PlatformLayer.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphCommands.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

DEFINE_RTTI(SceneGraph, Node, Spatial);

Node::Node(const SpatialId& id) : Spatial(id)
{
    m_factoryDesc = FactoryDesc(TYPE_RTTI.getName());
}

Node::~Node()
{
    while (m_childList.size())
    {
        SpatialPtr child = m_childList.front();
        m_childList.pop_front();
        if (child) child->linkParent(std::nullopt);
        child = nullptr;
    }
    m_childList.clear();
}

std::shared_ptr<Node> Node::create(const SpatialId& id)
{
    return std::make_shared<Node>(id);
}

std::shared_ptr<SpatialAssembler> Node::assembler() const
{
    return std::make_shared<NodeAssembler>(m_id);
}

void Node::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    assert(assembler);
    Spatial::assemble(assembler);
    if (auto nodeAssembler = std::dynamic_pointer_cast<NodeAssembler>(assembler))
    {
        for (const auto& child : m_childList)
        {
            if (!child) continue;
            if (child->persistenceLevel() == PersistenceLevel::Store)
            {
                nodeAssembler->child(child->id());
            }
            else
            {
                nodeAssembler->child(child);
            }
        }
    }
}

std::shared_ptr<SpatialDisassembler> Node::disassembler() const
{
    return std::make_shared<NodeDisassembler>();
}

void Node::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    Spatial::disassemble(disassembler);
    std::shared_ptr<NodeDisassembler> nodeDisassembler = std::dynamic_pointer_cast<NodeDisassembler>(disassembler);
    if (!nodeDisassembler) return;
    for (auto& [child_id, child_dto] : nodeDisassembler->children())
    {
        auto child_spatial = std::make_shared<QuerySpatial>(child_id)->dispatch();
        if (!child_spatial)
        {
            assert(child_dto.has_value());
            child_spatial = std::make_shared<RequestSpatialConstitution>(child_id, child_dto.value())->dispatch();
        }
        if (child_spatial) m_childList.push_back(child_spatial);
    }
}

std::shared_ptr<Node> Node::queryNode(const SpatialId& id)
{
    assert(id.rtti().isDerived(Node::TYPE_RTTI));
    return std::dynamic_pointer_cast<Node, Spatial>(std::make_shared<QuerySpatial>(id)->dispatch());
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

    SceneTraveler::TravelResult res = traveler->travelTo(thisSpatial());
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

error Node::attachChild(const SpatialPtr& child, const Matrix4& mxChildLocal)
{
    assert(child);
    if (FATAL_LOG_EXPR(child->getParent() != nullptr))
    {
        EventPublisher::enqueue(std::make_shared<NodeChildAttachmentFailed>(m_id, child->id(), ErrorCode::parentNode));
        return ErrorCode::parentNode; // must not have parent, must detach first!!
    }
    m_childList.push_back(child);
    child->linkParent(m_id);

    error er = child->setLocalTransform(mxChildLocal);

    if (!er) er = child->_updateSpatialRenderState();

    if (er)
    {
        m_childList.remove(child);
        child->linkParent(std::nullopt);
        EventPublisher::enqueue(std::make_shared<NodeChildAttachmentFailed>(m_id, child->id(), er));
        return er;
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<NodeChildAttached>(m_id, child));
    }
    return er;
}

error Node::detachChild(const SpatialPtr& child)
{
    assert(child);
    if (FATAL_LOG_EXPR(child->getParent() != thisSpatial()))
    {
        EventPublisher::enqueue(std::make_shared<NodeChildDetachmentFailed>(m_id, child->id(), ErrorCode::parentNode));
        return ErrorCode::parentNode;
    }
    child->linkParent(std::nullopt);

    error er = child->setLocalTransform(Matrix4::IDENTITY);

    er = child->_updateSpatialRenderState();

    // now, remove child, if child has no more reference, it will be deleted
    m_childList.remove(child);

    if (er)
    {
        EventPublisher::enqueue(std::make_shared<NodeChildDetachmentFailed>(m_id, child->id(), er));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<NodeChildDetached>(m_id, child->id()));
    }
    return er;
}

error Node::_updateLocalTransform(const Matrix4& mxLocal)
{
    m_mxLocalTransform = mxLocal;

    Matrix4 mxParent = Matrix4::IDENTITY;
    if (getParent())
    {
        mxParent = getParent()->getWorldTransform();
    }
    error er = _updateWorldData(mxParent);
    if (er) return er;

    if (testNotifyFlag(Notify_Location))
    {
        EventPublisher::enqueue(std::make_shared<SpatialLocationChanged>(m_id));
    }

    // propagate up
    er = _updateBoundData();

    return er;
}

error Node::_updateWorldData(const Matrix4& mxParentWorld)
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
    m_modelBound = BoundingVolume();
    if (m_childList.size())
    {
        ChildList::iterator iter = m_childList.begin();
        while (iter != m_childList.end())
        {
            if (m_modelBound.isEmpty())
            {
                m_modelBound = BoundingVolume::CreateFromTransform((*iter)->getModelBound(), (*iter)->getLocalTransform());
            }
            else
            {
                m_modelBound.Merge((*iter)->getLocalTransform(), (*iter)->getModelBound());
            }
            ++iter;
        }
    }
    m_worldBound = BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);

    if (testNotifyFlag(Notify_Bounding))
    {
        EventPublisher::enqueue(std::make_shared<SpatialBoundChanged>(m_id));
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
        EventPublisher::enqueue(std::make_shared<SpatialRenderStateChanged>(m_id));
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
