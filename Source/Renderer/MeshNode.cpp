#include "MeshNode.h"
#include "RenderablePrimitiveDtos.h"
#include "SkinMeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Renderer, MeshNode);

MeshNode::MeshNode(const std::string& name)
{
    m_name = name;
    m_mxLocalTransform = Matrix4::IDENTITY;
    m_mxRootRefTransform = Matrix4::IDENTITY;
    m_meshPrimitive = nullptr;
}

MeshNode::MeshNode(const MeshNode& node)
{
    m_name = node.m_name;
    m_mxLocalTransform = node.m_mxLocalTransform;
    m_mxRootRefTransform = node.m_mxRootRefTransform;
    if (node.m_meshPrimitive)
    {
        m_meshPrimitive = std::make_shared<MeshPrimitive>(*(node.m_meshPrimitive));
    }
    m_parentIndexInArray = node.m_parentIndexInArray;
}

MeshNode::MeshNode(MeshNode&& node)
{
    m_name = std::move(node.m_name);
    m_mxLocalTransform = std::move(node.m_mxLocalTransform);
    m_mxRootRefTransform = std::move(node.m_mxRootRefTransform);
    m_meshPrimitive = std::move(node.m_meshPrimitive);
    m_parentIndexInArray = std::move(node.m_parentIndexInArray);
}

MeshNode::~MeshNode()
{
    m_meshPrimitive = nullptr;
}

MeshNode& MeshNode::operator=(const MeshNode& node)
{
    m_name = node.m_name;
    m_mxLocalTransform = node.m_mxLocalTransform;
    m_mxRootRefTransform = node.m_mxRootRefTransform;
    if (node.m_meshPrimitive)
    {
        m_meshPrimitive = std::make_shared<MeshPrimitive>(*(node.m_meshPrimitive));
    }
    m_parentIndexInArray = node.m_parentIndexInArray;
    return *this;
}

MeshNode& MeshNode::operator=(MeshNode&& node)
{
    m_name = std::move(node.m_name);
    m_mxLocalTransform = std::move(node.m_mxLocalTransform);
    m_mxRootRefTransform = std::move(node.m_mxRootRefTransform);
    m_meshPrimitive = std::move(node.m_meshPrimitive);
    m_parentIndexInArray = std::move(node.m_parentIndexInArray);
    return *this;
}

GenericDto MeshNode::SerializeDto()
{
    MeshNodeDto dto;
    dto.Name() = m_name;
    dto.LocalTransform() = m_mxLocalTransform;
    dto.RootRefTransform() = m_mxRootRefTransform;
    if (m_meshPrimitive)
    {
        dto.TheMeshPrimitive() = m_meshPrimitive->SerializeDto();
    }
    if (m_parentIndexInArray)
    {
        dto.ParentIndexInArray() = m_parentIndexInArray.value();
    }
    return dto.ToGenericDto();
}

void MeshNode::SetMeshPrimitive(const MeshPrimitivePtr& mesh)
{
    m_meshPrimitive = mesh;
    if (const auto skin_prim = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(mesh))
    {
        skin_prim->BindOwnerMeshNode(*this);
    }
}

void MeshNode::SetParentIndexInArray(unsigned idx)
{
    m_parentIndexInArray = idx;
}
