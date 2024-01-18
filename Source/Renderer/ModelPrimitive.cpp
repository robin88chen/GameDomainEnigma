#include "ModelPrimitive.h"
#include "GameEngine/IRenderer.h"
#include "Platforms/PlatformLayer.h"
#include "RendererErrors.h"
#include "MathLib/Matrix4.h"
#include "RenderablePrimitiveDtos.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include <memory>

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::MathLib;
using namespace Enigma::Animators;
using namespace Enigma::Primitives;

DEFINE_RTTI(Renderer, ModelPrimitive, Primitive);

ModelPrimitive::ModelPrimitive(const PrimitiveId& id) : Primitive(id)
{
    m_factoryDesc = FactoryDesc(ModelPrimitive::TYPE_RTTI.getName());
    m_name = id.name();
    m_meshPrimitiveIndexCache.clear();
}

ModelPrimitive::ModelPrimitive(const PrimitiveId& id, const Engine::GenericDto& dto) : Primitive(id)
{
    ModelPrimitiveDto primDto = ModelPrimitiveDto::fromGenericDto(dto);
    m_factoryDesc = primDto.factoryDesc();
    m_nodeTree = MeshNodeTree(primDto.nodeTree());
    if (primDto.animatorId())
    {
        m_animator = Animator::queryAnimator(primDto.animatorId().value());
    }
}

/*ModelPrimitive::ModelPrimitive(const ModelPrimitive& prim) : Primitive(prim.m_id)
{
    m_name = prim.m_name;
    m_bound = prim.m_bound;
    m_mxPrimitiveWorld = prim.m_mxPrimitiveWorld;
    m_primitiveFlags = prim.m_primitiveFlags;
    m_nodeTree = prim.m_nodeTree;
    m_meshPrimitiveIndexCache = prim.m_meshPrimitiveIndexCache;
    ModelPrimitive::selectVisualTechnique(prim.m_selectedVisualTech);
}

ModelPrimitive::ModelPrimitive(ModelPrimitive&& prim) noexcept : Primitive(prim.m_id)
{
    m_name = std::move(prim.m_name);
    m_bound = std::move(prim.m_bound);
    m_mxPrimitiveWorld = std::move(prim.m_mxPrimitiveWorld);
    m_primitiveFlags = std::move(prim.m_primitiveFlags);
    m_nodeTree = std::move(prim.m_nodeTree);
    m_meshPrimitiveIndexCache = std::move(prim.m_meshPrimitiveIndexCache);
    ModelPrimitive::selectVisualTechnique(prim.m_selectedVisualTech);
}*/

ModelPrimitive::~ModelPrimitive()
{
    m_meshPrimitiveIndexCache.clear();
}

/*ModelPrimitive& ModelPrimitive::operator=(const ModelPrimitive& prim)
{
    m_id = prim.m_id;
    m_name = prim.m_name;
    m_bound = prim.m_bound;
    m_mxPrimitiveWorld = prim.m_mxPrimitiveWorld;
    m_primitiveFlags = prim.m_primitiveFlags;
    m_nodeTree = prim.m_nodeTree;
    m_meshPrimitiveIndexCache = prim.m_meshPrimitiveIndexCache;
    ModelPrimitive::selectVisualTechnique(prim.m_selectedVisualTech);
    return *this;
}

ModelPrimitive& ModelPrimitive::operator=(ModelPrimitive&& prim) noexcept
{
    m_id = prim.m_id;
    m_name = std::move(prim.m_name);
    m_nodeTree = std::move(prim.m_nodeTree);
    m_bound = std::move(prim.m_bound);
    m_mxPrimitiveWorld = std::move(prim.m_mxPrimitiveWorld);
    m_primitiveFlags = std::move(prim.m_primitiveFlags);
    m_meshPrimitiveIndexCache = std::move(prim.m_meshPrimitiveIndexCache);
    ModelPrimitive::selectVisualTechnique(prim.m_selectedVisualTech);
    return *this;
}*/

GenericDto ModelPrimitive::serializeDto() const
{
    ModelPrimitiveDto dto;
    dto.id() = m_id;
    dto.factoryDesc() = m_factoryDesc;
    dto.nodeTree() = m_nodeTree.serializeDto();
    if (m_animator) dto.animatorId() = m_animator->id();
    return dto.toGenericDto();
}

unsigned ModelPrimitive::getMeshPrimitiveCount()
{
    if (m_meshPrimitiveIndexCache.empty())
    {
        cacheMeshPrimitive();
    }
    return static_cast<unsigned>(m_meshPrimitiveIndexCache.size());
}

void ModelPrimitive::cacheMeshPrimitive()
{
    m_meshPrimitiveIndexCache.clear();
    const unsigned node_count = m_nodeTree.getMeshNodeCount();
    if (node_count == 0) return;
    for (unsigned i = 0; i < node_count; i++)
    {
        if (m_nodeTree.getMeshPrimitiveInNode(i))
        {
            m_meshPrimitiveIndexCache.push_back(i);
        }
    }
}

std::shared_ptr<MeshPrimitive> ModelPrimitive::getMeshPrimitive(unsigned cached_index)
{
    if (cached_index >= getMeshPrimitiveCount()) return nullptr;
    return m_nodeTree.getMeshPrimitiveInNode(m_meshPrimitiveIndexCache[cached_index]);
}

std::shared_ptr<MeshPrimitive> ModelPrimitive::findMeshPrimitive(const std::string& name)
{
    const unsigned mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return nullptr;
    for (unsigned int i = 0; i < mesh_count; i++)
    {
        std::shared_ptr<MeshPrimitive> mesh_prim = getMeshPrimitive(i);
        if ((mesh_prim) && (mesh_prim->getName() == name)) return mesh_prim;
    }
    return nullptr;
}

stdext::optional_ref<MeshNode> ModelPrimitive::getCachedMeshNode(unsigned cached_index)
{
    if (cached_index >= getMeshPrimitiveCount()) return std::nullopt;
    return m_nodeTree.getMeshNode(m_meshPrimitiveIndexCache[cached_index]);
}

void ModelPrimitive::updateMeshNodeLocalTransform(unsigned index, const MathLib::Matrix4& mxLocal)
{
    m_nodeTree.updateMeshNodeLocalTransform(m_mxPrimitiveWorld, index, mxLocal);
}

error ModelPrimitive::insertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState)
{
    const auto render = std::dynamic_pointer_cast<Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    m_mxPrimitiveWorld = mxWorld;
    if (testPrimitiveFlag(Primitive_UnRenderable)) return ErrorCode::ok;

    if (m_nodeTree.getMeshNodeCount() == 0) return ErrorCode::ok; // no mesh node
    const unsigned int mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return ErrorCode::ok; // no mesh primitive
    if (mesh_count == 1)
    {
        if (auto node = getCachedMeshNode(0))
        {
            Matrix4 mx = mxWorld * node.value().get().getRootRefTransform();
            if (getMeshPrimitive(0)) getMeshPrimitive(0)->insertToRendererWithTransformUpdating(renderer, mx, lightingState);
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (auto node = getCachedMeshNode(i))
            {
                Matrix4 mx = mxWorld * node.value().get().getRootRefTransform();
                if (getMeshPrimitive(i)) getMeshPrimitive(i)->insertToRendererWithTransformUpdating(renderer, mx, lightingState);
            }
        }
    }
    return ErrorCode::ok;
}

error ModelPrimitive::removeFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer)
{
    const auto render = std::dynamic_pointer_cast<Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    if (m_nodeTree.getMeshNodeCount() == 0) return ErrorCode::ok; // no mesh node
    const unsigned int mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return ErrorCode::ok; // no mesh primitive
    if (mesh_count == 1)
    {
        if (getMeshPrimitive(0))
        {
            getMeshPrimitive(0)->removeFromRenderer(renderer);
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (getMeshPrimitive(i))
            {
                getMeshPrimitive(i)->removeFromRenderer(renderer);
            }
        }
    }
    return ErrorCode::ok;
}

void ModelPrimitive::updateWorldTransform(const MathLib::Matrix4& mxWorld)
{
    m_mxPrimitiveWorld = mxWorld;

    if (m_nodeTree.getMeshNodeCount() == 0) return;  // no mesh node
    const unsigned int mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return; // no mesh primitive

    if (mesh_count == 1)
    {
        if (auto node = getCachedMeshNode(0))
        {
            if (getMeshPrimitive(0)) getMeshPrimitive(0)->updateWorldTransform(mxWorld * node.value().get().getRootRefTransform());
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (auto node = getCachedMeshNode(i))
            {
                if (getMeshPrimitive(i)) getMeshPrimitive(i)->updateWorldTransform(mxWorld * node.value().get().getRootRefTransform());
            }
        }
    }
}

void ModelPrimitive::calculateBoundingVolume(bool axis_align)
{
    m_bound = BoundingVolume{ Box3::UNIT_BOX };
    const unsigned int mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return;

    for (unsigned int i = 0; i < mesh_count; i++)
    {
        auto mesh_node = getCachedMeshNode(i);
        if (!mesh_node) continue;
        std::shared_ptr<MeshPrimitive> mesh_prim = mesh_node.value().get().getMeshPrimitive();
        if (!mesh_prim) continue;
        if (!mesh_prim->testPrimitiveFlag(Primitive_UnBound))
        {
            mesh_prim->calculateBoundingVolume(axis_align);
            m_bound.Merge(mesh_node.value().get().getRootRefTransform(), mesh_prim->getBoundingVolume());
        }
    }
}

void ModelPrimitive::selectVisualTechnique(const std::string& techniqueName)
{
    Primitive::selectVisualTechnique(techniqueName);

    if (m_nodeTree.getMeshNodeCount() == 0) return;  // no mesh node
    const unsigned int mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return; // no mesh primitive

    if (mesh_count == 1)
    {
        if (getMeshPrimitive(0))
        {
            getMeshPrimitive(0)->selectVisualTechnique(techniqueName);
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (getMeshPrimitive(i))
            {
                getMeshPrimitive(i)->selectVisualTechnique(techniqueName);
            }
        }
    }
}

void ModelPrimitive::enumAnimatorListDeep(std::list<std::shared_ptr<Engine::Animator>>& resultList)
{
    if (m_animator) resultList.push_back(m_animator);
    if (m_nodeTree.getMeshNodeCount() == 0) return;  // no mesh node
    unsigned int mesh_count = getMeshPrimitiveCount();
    if (mesh_count == 0) return;

    for (unsigned int i = 0; i < mesh_count; i++)
    {
        if (getMeshPrimitive(i))
        {
            getMeshPrimitive(i)->enumAnimatorListDeep(resultList);
        }
    }
}
