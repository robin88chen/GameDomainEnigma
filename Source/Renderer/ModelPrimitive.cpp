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

DEFINE_RTTI(Renderer, ModelPrimitive, Primitive);

ModelPrimitive::ModelPrimitive(const std::string& name) : Primitive()
{
    m_factoryDesc = FactoryDesc(ModelPrimitive::TYPE_RTTI.getName());
    m_name = name;
    m_meshPrimitiveIndexCache.clear();
}

ModelPrimitive::ModelPrimitive(const ModelPrimitive& prim)
{
    m_name = prim.m_name;
    m_bound = prim.m_bound;
    m_mxPrimitiveWorld = prim.m_mxPrimitiveWorld;
    m_primitiveFlags = prim.m_primitiveFlags;
    m_nodeTree = prim.m_nodeTree;
    m_meshPrimitiveIndexCache = prim.m_meshPrimitiveIndexCache;
    ModelPrimitive::SelectVisualTechnique(prim.m_selectedVisualTech);
}

ModelPrimitive::ModelPrimitive(ModelPrimitive&& prim) noexcept
{
    m_name = std::move(prim.m_name);
    m_bound = std::move(prim.m_bound);
    m_mxPrimitiveWorld = std::move(prim.m_mxPrimitiveWorld);
    m_primitiveFlags = std::move(prim.m_primitiveFlags);
    m_nodeTree = std::move(prim.m_nodeTree);
    m_meshPrimitiveIndexCache = std::move(prim.m_meshPrimitiveIndexCache);
    ModelPrimitive::SelectVisualTechnique(prim.m_selectedVisualTech);
}

ModelPrimitive::~ModelPrimitive()
{
    m_meshPrimitiveIndexCache.clear();
}

ModelPrimitive& ModelPrimitive::operator=(const ModelPrimitive& prim)
{
    m_name = prim.m_name;
    m_bound = prim.m_bound;
    m_mxPrimitiveWorld = prim.m_mxPrimitiveWorld;
    m_primitiveFlags = prim.m_primitiveFlags;
    m_nodeTree = prim.m_nodeTree;
    m_meshPrimitiveIndexCache = prim.m_meshPrimitiveIndexCache;
    ModelPrimitive::SelectVisualTechnique(prim.m_selectedVisualTech);
    return *this;
}

ModelPrimitive& ModelPrimitive::operator=(ModelPrimitive&& prim) noexcept
{
    m_name = std::move(prim.m_name);
    m_nodeTree = std::move(prim.m_nodeTree);
    m_bound = std::move(prim.m_bound);
    m_mxPrimitiveWorld = std::move(prim.m_mxPrimitiveWorld);
    m_primitiveFlags = std::move(prim.m_primitiveFlags);
    m_meshPrimitiveIndexCache = std::move(prim.m_meshPrimitiveIndexCache);
    ModelPrimitive::SelectVisualTechnique(prim.m_selectedVisualTech);
    return *this;
}

GenericDto ModelPrimitive::serializeDto() const
{
    ModelPrimitiveDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.Name() = m_name;
    dto.TheNodeTree() = m_nodeTree.serializeDto();
    if (auto ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(m_animator))
        dto.TheAnimator() = ani->serializeDto().ToGenericDto();
    return dto.ToGenericDto();
}

unsigned ModelPrimitive::GetMeshPrimitiveCount()
{
    if (m_meshPrimitiveIndexCache.empty())
    {
        CacheMeshPrimitive();
    }
    return static_cast<unsigned>(m_meshPrimitiveIndexCache.size());
}

void ModelPrimitive::CacheMeshPrimitive()
{
    m_meshPrimitiveIndexCache.clear();
    const unsigned node_count = m_nodeTree.GetMeshNodeCount();
    if (node_count == 0) return;
    for (unsigned i = 0; i < node_count; i++)
    {
        if (m_nodeTree.GetMeshPrimitiveInNode(i))
        {
            m_meshPrimitiveIndexCache.push_back(i);
        }
    }
}

MeshPrimitivePtr ModelPrimitive::GetMeshPrimitive(unsigned cached_index)
{
    if (cached_index >= GetMeshPrimitiveCount()) return nullptr;
    return m_nodeTree.GetMeshPrimitiveInNode(m_meshPrimitiveIndexCache[cached_index]);
}

MeshPrimitivePtr ModelPrimitive::FindMeshPrimitive(const std::string& name)
{
    const unsigned mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return nullptr;
    for (unsigned int i = 0; i < mesh_count; i++)
    {
        MeshPrimitivePtr mesh_prim = GetMeshPrimitive(i);
        if ((mesh_prim) && (mesh_prim->getName() == name)) return mesh_prim;
    }
    return nullptr;
}

stdext::optional_ref<MeshNode> ModelPrimitive::GetCachedMeshNode(unsigned cached_index)
{
    if (cached_index >= GetMeshPrimitiveCount()) return std::nullopt;
    return m_nodeTree.GetMeshNode(m_meshPrimitiveIndexCache[cached_index]);
}

void ModelPrimitive::UpdateMeshNodeLocalTransform(unsigned index, const MathLib::Matrix4& mxLocal)
{
    m_nodeTree.UpdateMeshNodeLocalTransform(m_mxPrimitiveWorld, index, mxLocal);
}

error ModelPrimitive::InsertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState)
{
    const auto render = std::dynamic_pointer_cast<Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    m_mxPrimitiveWorld = mxWorld;
    if (TestPrimitiveFlag(Primitive_UnRenderable)) return ErrorCode::ok;

    if (m_nodeTree.GetMeshNodeCount() == 0) return ErrorCode::ok; // no mesh node
    const unsigned int mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return ErrorCode::ok; // no mesh primitive
    if (mesh_count == 1)
    {
        if (auto node = GetCachedMeshNode(0))
        {
            Matrix4 mx = mxWorld * node.value().get().GetRootRefTransform();
            if (GetMeshPrimitive(0)) GetMeshPrimitive(0)->InsertToRendererWithTransformUpdating(renderer, mx, lightingState);
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (auto node = GetCachedMeshNode(i))
            {
                Matrix4 mx = mxWorld * node.value().get().GetRootRefTransform();
                if (GetMeshPrimitive(i)) GetMeshPrimitive(i)->InsertToRendererWithTransformUpdating(renderer, mx, lightingState);
            }
        }
    }
    return ErrorCode::ok;
}

error ModelPrimitive::RemoveFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer)
{
    const auto render = std::dynamic_pointer_cast<Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    if (m_nodeTree.GetMeshNodeCount() == 0) return ErrorCode::ok; // no mesh node
    const unsigned int mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return ErrorCode::ok; // no mesh primitive
    if (mesh_count == 1)
    {
        if (GetMeshPrimitive(0))
        {
            GetMeshPrimitive(0)->RemoveFromRenderer(renderer);
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (GetMeshPrimitive(i))
            {
                GetMeshPrimitive(i)->RemoveFromRenderer(renderer);
            }
        }
    }
    return ErrorCode::ok;
}

void ModelPrimitive::UpdateWorldTransform(const MathLib::Matrix4& mxWorld)
{
    m_mxPrimitiveWorld = mxWorld;

    if (m_nodeTree.GetMeshNodeCount() == 0) return;  // no mesh node
    const unsigned int mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return; // no mesh primitive

    if (mesh_count == 1)
    {
        if (auto node = GetCachedMeshNode(0))
        {
            if (GetMeshPrimitive(0)) GetMeshPrimitive(0)->UpdateWorldTransform(mxWorld * node.value().get().GetRootRefTransform());
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (auto node = GetCachedMeshNode(i))
            {
                if (GetMeshPrimitive(i)) GetMeshPrimitive(i)->UpdateWorldTransform(mxWorld * node.value().get().GetRootRefTransform());
            }
        }
    }
}

void ModelPrimitive::CalculateBoundingVolume(bool axis_align)
{
    m_bound = BoundingVolume{ Box3::UNIT_BOX };
    const unsigned int mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return;

    for (unsigned int i = 0; i < mesh_count; i++)
    {
        auto mesh_node = GetCachedMeshNode(i);
        if (!mesh_node) continue;
        MeshPrimitivePtr mesh_prim = mesh_node.value().get().GetMeshPrimitive();
        if (!mesh_prim) continue;
        if (!mesh_prim->TestPrimitiveFlag(Primitive_UnBound))
        {
            mesh_prim->CalculateBoundingVolume(axis_align);
            m_bound.Merge(mesh_node.value().get().GetRootRefTransform(), mesh_prim->GetBoundingVolume());
        }
    }
}

void ModelPrimitive::SelectVisualTechnique(const std::string& techniqueName)
{
    Primitive::SelectVisualTechnique(techniqueName);

    if (m_nodeTree.GetMeshNodeCount() == 0) return;  // no mesh node
    const unsigned int mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return; // no mesh primitive

    if (mesh_count == 1)
    {
        if (GetMeshPrimitive(0))
        {
            GetMeshPrimitive(0)->SelectVisualTechnique(techniqueName);
        }
    }
    else
    {
        for (unsigned int i = 0; i < mesh_count; i++)
        {
            if (GetMeshPrimitive(i))
            {
                GetMeshPrimitive(i)->SelectVisualTechnique(techniqueName);
            }
        }
    }
}

void ModelPrimitive::EnumAnimatorListDeep(std::list<std::shared_ptr<Engine::Animator>>& resultList)
{
    if (m_animator) resultList.push_back(m_animator);
    if (m_nodeTree.GetMeshNodeCount() == 0) return;  // no mesh node
    unsigned int mesh_count = GetMeshPrimitiveCount();
    if (mesh_count == 0) return;

    for (unsigned int i = 0; i < mesh_count; i++)
    {
        if (GetMeshPrimitive(i))
        {
            GetMeshPrimitive(i)->EnumAnimatorListDeep(resultList);
        }
    }
}
