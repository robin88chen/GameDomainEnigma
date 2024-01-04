/*********************************************************************
 * \file   MeshNode.h
 * \brief  Mesh Node, value object, used by value
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_NODE_H
#define _MESH_NODE_H

#include "Frameworks/Rtti.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/GenericDto.h"
#include "MathLib/Matrix4.h"
#include <string>

namespace Enigma::Renderer
{
    class MeshPrimitive;
    class MeshNode
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        MeshNode(const std::string& name);
        MeshNode(const MeshNode& node);
        MeshNode(MeshNode&& node) noexcept;
        ~MeshNode();
        MeshNode& operator=(const MeshNode& node);
        MeshNode& operator=(MeshNode&& node) noexcept;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        Engine::GenericDto serializeDto() const;

        const std::string& getName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }
        void SetT_PosTransform(const MathLib::Matrix4& mx) { m_mxT_PosTransform = mx; }
        const MathLib::Matrix4& getLocalTransform() const { return m_mxLocalTransform; }
        void setLocalTransform(const MathLib::Matrix4& mx) { m_mxLocalTransform = mx; }
        const MathLib::Matrix4& GetRootRefTransform() const { return m_mxRootRefTransform; }
        void SetRootRefTransform(const MathLib::Matrix4& mx); // { m_mxRootRefTransform = mx; }
        const std::shared_ptr<MeshPrimitive>& GetMeshPrimitive() const { return m_meshPrimitive; }
        std::shared_ptr<MeshPrimitive> GetMeshPrimitive() { return m_meshPrimitive; }
        void SetMeshPrimitive(const std::shared_ptr<MeshPrimitive>& mesh);
        std::optional<unsigned> GetParentIndexInArray() const { return m_parentIndexInArray; }
        void SetParentIndexInArray(unsigned idx);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        MathLib::Matrix4 m_mxT_PosTransform; ///< mesh node local original T-Pos transform
        MathLib::Matrix4 m_mxLocalTransform;   ///< mesh node local transform (reference: parent node)
        MathLib::Matrix4 m_mxRootRefTransform; ///< mesh node transform (reference: model root)
        std::shared_ptr<MeshPrimitive> m_meshPrimitive;   ///< mesh primitive
        std::optional<unsigned> m_parentIndexInArray;  ///< parent index in mesh node array
        bool m_hasSkinMeshPrimitive;
    };
}

#endif // _MESH_NODE_H
