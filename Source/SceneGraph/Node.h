/*********************************************************************
 * \file   Node.h
 * \brief  Scene Graph Node, entity, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef NODE_H
#define NODE_H

#include "Spatial.h"
#include <list>
#include "Frameworks/Rtti.h"
#include <string>

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    class NodeDto;

    /** scene graph node class */
    class Node : public Spatial
    {
        DECLARE_EN_RTTI;
    public:
        using ChildList = std::list<std::shared_ptr<Spatial>>;

    public:
        Node(const SpatialId& id);
        Node(const Node&) = delete;
        Node(Node&&) = delete;
        virtual ~Node() override;
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;

        static std::shared_ptr<Node> create(const SpatialId& id);

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler) override;

        static std::shared_ptr<Node> queryNode(const SpatialId& id);

        //todo: remove
        //virtual Engine::GenericDtoCollection serializeFlattenedTree();

        /** on cull visible, used by culler, for compute visible set, recursive calling children's "CullingVisibleSet"  */
        virtual error onCullingVisible(Culler* culler, bool noCull) override;
        virtual bool canVisited() override { return true; };

        virtual SceneTraveler::TravelResult visitBy(SceneTraveler* traveler) override;

        /** attach child spatial object, setting child's local transform, then update */
        virtual error attachChild(const SpatialPtr& child, const MathLib::Matrix4& mxChildLocal);
        /** detach child spatial object */
        virtual error detachChild(const SpatialPtr& child);
        /** get child list */
        virtual const ChildList& getChildList() { return m_childList; };

        /** @name inner public functions    */
        //@{
        virtual error _updateLocalTransform(const MathLib::Matrix4& mxLocal) override;
        virtual error _updateWorldData(const MathLib::Matrix4& mxParentWorld) override;
        virtual error _updateBoundData() override;

        // notify parent to update me!!
        virtual error _propagateSpatialRenderState() override;
        // call by parent
        virtual error _updateSpatialRenderState() override;
        //@}

        /** enum animator list deep, including geometry's animator */
        //todo : animators
        //virtual void enumAnimatorListDeep(AnimatorList& resultList) override;

        std::shared_ptr<Node> thisNode()
        {
            return std::dynamic_pointer_cast<Node, Spatial>(shared_from_this());
        }
        std::shared_ptr<const Node> thisNode() const
        {
            return std::dynamic_pointer_cast<const Node, const Spatial>(shared_from_this());
        }

    protected:
        //NodeDto serializeNodeDto();

    protected:
        //todo : rethink -- mutex for lock list??
        ChildList m_childList;
    };
};


#endif // NODE_H
