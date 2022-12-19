#include "MeshNode.h"

using namespace Enigma::Renderer;
using namespace Enigma::MathLib;

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

void MeshNode::SetMeshPrimitive(const MeshPrimitivePtr& mesh)
{
    m_meshPrimitive = mesh;
    //todo : skin mesh
    /*SkinMeshPrimitivePtr skin_prim = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(prim);
    if (skin_prim)
    {
        skin_prim->SetOwnerMeshNode(std::dynamic_pointer_cast<MeshNode, Object>(shared_from_this()));
    }*/
}

void MeshNode::SetParentIndexInArray(unsigned idx)
{
    m_parentIndexInArray = idx;
}
