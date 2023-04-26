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
    m_mxT_PosTransform = Matrix4::IDENTITY;
    m_mxLocalTransform = Matrix4::IDENTITY;
    m_mxRootRefTransform = Matrix4::IDENTITY;
    m_meshPrimitive = nullptr;
    m_hasSkinMeshPrimitive = false;
}

MeshNode::MeshNode(const MeshNode& node)
{
    m_name = node.m_name;
    m_mxT_PosTransform = node.m_mxT_PosTransform;
    m_mxLocalTransform = node.m_mxLocalTransform;
    m_mxRootRefTransform = node.m_mxRootRefTransform;
    m_hasSkinMeshPrimitive = node.m_hasSkinMeshPrimitive;
    if (node.m_meshPrimitive)
    {
        if (auto skinmesh = dynamic_cast<SkinMeshPrimitive*>(node.m_meshPrimitive.get()))
        {
            m_meshPrimitive = std::make_shared<SkinMeshPrimitive>(*skinmesh);
        }
        else
        {
            m_meshPrimitive = std::make_shared<MeshPrimitive>(*(node.m_meshPrimitive));
        }
    }
    m_parentIndexInArray = node.m_parentIndexInArray;
}

MeshNode::MeshNode(MeshNode&& node)
{
    m_name = std::move(node.m_name);
    m_mxT_PosTransform = std::move(node.m_mxT_PosTransform);
    m_mxLocalTransform = std::move(node.m_mxLocalTransform);
    m_mxRootRefTransform = std::move(node.m_mxRootRefTransform);
    m_meshPrimitive = std::move(node.m_meshPrimitive);
    m_parentIndexInArray = std::move(node.m_parentIndexInArray);
    m_hasSkinMeshPrimitive = std::move(node.m_hasSkinMeshPrimitive);
}

MeshNode::~MeshNode()
{
    m_meshPrimitive = nullptr;
}

MeshNode& MeshNode::operator=(const MeshNode& node)
{
    m_name = node.m_name;
    m_mxT_PosTransform = node.m_mxT_PosTransform;
    m_mxLocalTransform = node.m_mxLocalTransform;
    m_mxRootRefTransform = node.m_mxRootRefTransform;
    m_hasSkinMeshPrimitive = node.m_hasSkinMeshPrimitive;
    if (node.m_meshPrimitive)
    {
        if (auto skinmesh = dynamic_cast<SkinMeshPrimitive*>(node.m_meshPrimitive.get()))
        {
            m_meshPrimitive = std::make_shared<SkinMeshPrimitive>(*skinmesh);
        }
        else
        {
            m_meshPrimitive = std::make_shared<MeshPrimitive>(*(node.m_meshPrimitive));
        }
    }
    m_parentIndexInArray = node.m_parentIndexInArray;
    return *this;
}

MeshNode& MeshNode::operator=(MeshNode&& node)
{
    m_name = std::move(node.m_name);
    m_mxT_PosTransform = std::move(node.m_mxT_PosTransform);
    m_mxLocalTransform = std::move(node.m_mxLocalTransform);
    m_mxRootRefTransform = std::move(node.m_mxRootRefTransform);
    m_meshPrimitive = std::move(node.m_meshPrimitive);
    m_parentIndexInArray = std::move(node.m_parentIndexInArray);
    m_hasSkinMeshPrimitive = std::move(node.m_hasSkinMeshPrimitive);
    return *this;
}

GenericDto MeshNode::SerializeDto()
{
    MeshNodeDto dto;
    dto.Name() = m_name;
    dto.LocalT_PosTransform() = m_mxT_PosTransform;
    //dto.RootRefTransform() = m_mxRootRefTransform;
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
        m_hasSkinMeshPrimitive = true;
        skin_prim->BindOwnerRootRefTransform(m_mxRootRefTransform);
    }
}

void MeshNode::SetParentIndexInArray(unsigned idx)
{
    m_parentIndexInArray = idx;
}

void MeshNode::SetRootRefTransform(const MathLib::Matrix4& mx)
{
    m_mxRootRefTransform = mx;
    if ((m_hasSkinMeshPrimitive) && (m_meshPrimitive))
    {
        if (const auto skin_prim = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(m_meshPrimitive))
        {
            skin_prim->BindOwnerRootRefTransform(m_mxRootRefTransform);
        }
    }
}
