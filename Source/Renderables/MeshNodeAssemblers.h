/*********************************************************************
 * \file   MeshNodeAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef MESH_NODE_ASSEMBLERS_H
#define MESH_NODE_ASSEMBLERS_H

#include "GameEngine/FactoryDesc.h"
#include "GameEngine/GenericDto.h"
#include "MathLib/Matrix4.h"
#include "Primitives/PrimitiveId.h"
#include <optional>
#include <string>

namespace Enigma::Renderables
{
    class MeshNode;

    class MeshNodeAssembler
    {
    public:
        MeshNodeAssembler();
        ~MeshNodeAssembler() = default;
        MeshNodeAssembler(const MeshNodeAssembler&) = default;
        MeshNodeAssembler(MeshNodeAssembler&&) = default;
        MeshNodeAssembler& operator=(const MeshNodeAssembler&) = default;
        MeshNodeAssembler& operator=(MeshNodeAssembler&&) = default;

        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }
        void name(const std::string& name) { m_name = name; }

        void localT_PosTransform(const MathLib::Matrix4& transform) { m_localT_PosTransform = transform; }
        void meshPrimitiveId(const Primitives::PrimitiveId& id) { m_meshPrimitiveId = id; }
        void parentIndexInArray(unsigned index) { m_parentIndexInArray = index; }

        Engine::GenericDto assemble() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        MathLib::Matrix4 m_localT_PosTransform;
        std::optional<Primitives::PrimitiveId> m_meshPrimitiveId;
        std::optional<unsigned> m_parentIndexInArray;
    };
    class MeshNodeDisassembler
    {
    public:
        MeshNodeDisassembler();
        ~MeshNodeDisassembler() = default;
        MeshNodeDisassembler(const MeshNodeDisassembler&) = default;
        MeshNodeDisassembler(MeshNodeDisassembler&&) = default;
        MeshNodeDisassembler& operator=(const MeshNodeDisassembler&) = default;
        MeshNodeDisassembler& operator=(MeshNodeDisassembler&&) = default;

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] const std::string& name() const { return m_name; }
        [[nodiscard]] const MathLib::Matrix4& localT_PosTransform() const { return m_localT_PosTransform; }
        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& meshPrimitiveId() const { return m_meshPrimitiveId; }
        [[nodiscard]] const std::optional<unsigned>& parentIndexInArray() const { return m_parentIndexInArray; }

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        MathLib::Matrix4 m_localT_PosTransform;
        std::optional<Primitives::PrimitiveId> m_meshPrimitiveId;
        std::optional<unsigned> m_parentIndexInArray;
    };
    class MeshNodeTreeAssembler
    {
    public:
        MeshNodeTreeAssembler();
        ~MeshNodeTreeAssembler() = default;
        MeshNodeTreeAssembler(const MeshNodeTreeAssembler&) = default;
        MeshNodeTreeAssembler(MeshNodeTreeAssembler&&) = default;
        MeshNodeTreeAssembler& operator=(const MeshNodeTreeAssembler&) = default;
        MeshNodeTreeAssembler& operator=(MeshNodeTreeAssembler&&) = default;

        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }
        void addNodeWithParentName(const MeshNode& node, const std::string& parent_name);
        void addNode(const MeshNode& node);
        void addNode(const std::string& node_name, const std::shared_ptr<MeshNodeAssembler>& assembler);

        Engine::GenericDto assemble() const;

    protected:
        std::optional<unsigned> findNodeIndex(const std::string& name) const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::vector<std::string> m_nodeNames;
        std::vector<std::shared_ptr<MeshNodeAssembler>> m_nodesAssemblers;
    };
    class MeshNodeTreeDisassembler
    {
    public:
        MeshNodeTreeDisassembler();
        ~MeshNodeTreeDisassembler() = default;
        MeshNodeTreeDisassembler(const MeshNodeTreeDisassembler&) = default;
        MeshNodeTreeDisassembler(MeshNodeTreeDisassembler&&) = default;
        MeshNodeTreeDisassembler& operator=(const MeshNodeTreeDisassembler&) = default;
        MeshNodeTreeDisassembler& operator=(MeshNodeTreeDisassembler&&) = default;

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] const std::vector<MeshNode>& nodes() const { return m_nodes; }

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::vector<MeshNode> m_nodes;
    };
}

#endif // MESH_NODE_ASSEMBLERS_H
