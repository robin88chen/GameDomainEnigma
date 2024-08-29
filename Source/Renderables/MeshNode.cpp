#include "MeshNode.h"
#include "SkinMeshPrimitive.h"
#include "MeshNodeAssemblers.h"

using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Primitives;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Renderables, MeshNode);

MeshNode::MeshNode() : m_factoryDesc(TYPE_RTTI.getName())
{
    m_name = "";
    m_mxT_PosTransform = Matrix4::IDENTITY;
    m_mxLocalTransform = Matrix4::IDENTITY;
    m_mxRootRefTransform = Matrix4::IDENTITY;
    m_meshPrimitive = nullptr;
    m_hasSkinMeshPrimitive = false;

}

MeshNode::MeshNode(const std::string& name) : m_factoryDesc(TYPE_RTTI.getName())
{
    m_name = name;
    m_mxT_PosTransform = Matrix4::IDENTITY;
    m_mxLocalTransform = Matrix4::IDENTITY;
    m_mxRootRefTransform = Matrix4::IDENTITY;
    m_meshPrimitive = nullptr;
    m_hasSkinMeshPrimitive = false;
}

MeshNode::MeshNode(const MeshNode& node) : m_factoryDesc(node.factoryDesc())
{
    m_name = node.m_name;
    m_mxT_PosTransform = node.m_mxT_PosTransform;
    m_mxLocalTransform = node.m_mxLocalTransform;
    m_mxRootRefTransform = node.m_mxRootRefTransform;
    m_hasSkinMeshPrimitive = node.m_hasSkinMeshPrimitive;
    m_meshPrimitive = node.m_meshPrimitive;
    m_parentIndexInArray = node.m_parentIndexInArray;
}

MeshNode::MeshNode(MeshNode&& node) noexcept : m_factoryDesc(std::move(node.factoryDesc()))
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
    if (this == &node) return *this;
    m_factoryDesc = node.m_factoryDesc;
    m_name = node.m_name;
    m_mxT_PosTransform = node.m_mxT_PosTransform;
    m_mxLocalTransform = node.m_mxLocalTransform;
    m_mxRootRefTransform = node.m_mxRootRefTransform;
    m_hasSkinMeshPrimitive = node.m_hasSkinMeshPrimitive;
    m_meshPrimitive = node.m_meshPrimitive;
    m_parentIndexInArray = node.m_parentIndexInArray;
    return *this;
}

MeshNode& MeshNode::operator=(MeshNode&& node) noexcept
{
    m_factoryDesc = std::move(node.m_factoryDesc);
    m_name = std::move(node.m_name);
    m_mxT_PosTransform = std::move(node.m_mxT_PosTransform);
    m_mxLocalTransform = std::move(node.m_mxLocalTransform);
    m_mxRootRefTransform = std::move(node.m_mxRootRefTransform);
    m_meshPrimitive = std::move(node.m_meshPrimitive);
    m_parentIndexInArray = std::move(node.m_parentIndexInArray);
    m_hasSkinMeshPrimitive = std::move(node.m_hasSkinMeshPrimitive);
    return *this;
}

void MeshNode::assemble(const std::shared_ptr<MeshNodeAssembler>& assembler) const
{
    assembler->factoryDesc(m_factoryDesc);
    assembler->name(m_name);
    assembler->localT_PosTransform(m_mxT_PosTransform);
    if (m_meshPrimitive)
    {
        assembler->meshPrimitiveId(m_meshPrimitive->id().origin());
    }
    if (m_parentIndexInArray)
    {
        assembler->parentIndexInArray(m_parentIndexInArray.value());
    }
}

void MeshNode::disassemble(const std::shared_ptr<MeshNodeDisassembler>& disassembler)
{
    m_name = disassembler->name();
    m_mxT_PosTransform = disassembler->localT_PosTransform();
    m_mxLocalTransform = m_mxT_PosTransform;
    m_mxRootRefTransform = m_mxT_PosTransform;
    //m_mxRootRefTransform = mesh_node_dto.RootRefTransform();
    m_hasSkinMeshPrimitive = false;
    if (disassembler->meshPrimitiveId())
    {
        m_meshPrimitive = std::dynamic_pointer_cast<MeshPrimitive>(Primitive::queryPrimitive(disassembler->meshPrimitiveId().value().next()));
        if ((m_meshPrimitive) && (m_meshPrimitive->typeInfo().isDerived(SkinMeshPrimitive::TYPE_RTTI)))
        {
            m_hasSkinMeshPrimitive = true;
        }
    }
    if (disassembler->parentIndexInArray())
    {
        m_parentIndexInArray = disassembler->parentIndexInArray().value();
    }
}

void MeshNode::setMeshPrimitive(const std::shared_ptr<MeshPrimitive>& mesh)
{
    m_meshPrimitive = mesh;
    if (const auto skin_prim = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(mesh))
    {
        m_hasSkinMeshPrimitive = true;
        skin_prim->bindOwnerRootRefTransform(m_mxRootRefTransform);
    }
}

void MeshNode::setParentIndexInArray(unsigned idx)
{
    m_parentIndexInArray = idx;
}

void MeshNode::setRootRefTransform(const Matrix4& mx)
{
    m_mxRootRefTransform = mx;
    if ((m_hasSkinMeshPrimitive) && (m_meshPrimitive))
    {
        if (const auto skin_prim = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(m_meshPrimitive))
        {
            skin_prim->bindOwnerRootRefTransform(m_mxRootRefTransform);
        }
    }
}
