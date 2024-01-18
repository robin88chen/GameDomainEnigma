#include "MeshNode.h"
#include "RenderablePrimitiveDtos.h"
#include "SkinMeshPrimitive.h"

using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Primitives;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Renderables, MeshNode);

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
    if (node.m_meshPrimitive)
    {
        if (auto skinmesh = dynamic_cast<SkinMeshPrimitive*>(node.m_meshPrimitive.get()))
        {
            //m_meshPrimitive = std::make_shared<SkinMeshPrimitive>(*skinmesh);
        }
        else
        {
            //m_meshPrimitive = std::make_shared<MeshPrimitive>(*(node.m_meshPrimitive));
        }
    }
    m_parentIndexInArray = node.m_parentIndexInArray;
}

MeshNode::MeshNode(const GenericDto& dto) : m_factoryDesc(TYPE_RTTI.getName())
{
    MeshNodeDto mesh_node_dto = MeshNodeDto::fromGenericDto(dto);
    m_name = mesh_node_dto.name();
    m_mxT_PosTransform = mesh_node_dto.localT_PosTransform();
    m_mxLocalTransform = m_mxT_PosTransform;
    m_mxRootRefTransform = m_mxT_PosTransform;
    //m_mxRootRefTransform = mesh_node_dto.RootRefTransform();
    if (mesh_node_dto.meshPrimitiveId())
    {
        m_meshPrimitive = std::dynamic_pointer_cast<MeshPrimitive>(Primitive::queryPrimitive(mesh_node_dto.meshPrimitiveId().value()));
    }
    if (mesh_node_dto.parentIndexInArray())
    {
        m_parentIndexInArray = mesh_node_dto.parentIndexInArray().value();
    }
    m_hasSkinMeshPrimitive = false;
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
    if (node.m_meshPrimitive)
    {
        if (auto skinmesh = dynamic_cast<SkinMeshPrimitive*>(node.m_meshPrimitive.get()))
        {
            //m_meshPrimitive = std::make_shared<SkinMeshPrimitive>(*skinmesh);
        }
        else
        {
            //m_meshPrimitive = std::make_shared<MeshPrimitive>(*(node.m_meshPrimitive));
        }
    }
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

GenericDto MeshNode::serializeDto() const
{
    MeshNodeDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.name() = m_name;
    dto.localT_PosTransform() = m_mxT_PosTransform;
    //dto.RootRefTransform() = m_mxRootRefTransform;
    if (m_meshPrimitive)
    {
        dto.meshPrimitiveId() = m_meshPrimitive->id();
    }
    if (m_parentIndexInArray)
    {
        dto.parentIndexInArray() = m_parentIndexInArray.value();
    }
    return dto.toGenericDto();
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
