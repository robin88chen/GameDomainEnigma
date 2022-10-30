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
#include <string>

namespace Enigma::Engine
{
    template <class T> class ContractedLinkageResolver;
}

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    class NodeContract;

    /** scene graph node class */
    class Node : public Spatial
    {
    public:
        static Frameworks::Rtti TYPE_RTTI;
        const Frameworks::Rtti& TypeInfo() const { return TYPE_RTTI; }

    public:
        using ChildList = std::list<SpatialPtr>;

    public:
        Node(const std::string& name);
        Node(const NodeContract& contract);
        Node(const Node&) = delete;
        Node(Node&&) = delete;
        virtual ~Node();
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;

        NodeContract SerializeContract();
        void ResolveContractedLinkage(const NodeContract& contract, Engine::ContractedLinkageResolver<Spatial>& resolver);

        /** on cull visible, used by culler, for compute visible set, recursive calling children's "CullingVisibleSet"  */
        virtual error OnCullingVisible(Culler* culler, bool noCull) override;
        virtual bool CanVisited() override { return true; };

        virtual SceneTraveler::TravelResult VisitBy(SceneTraveler* traveler) override;

        /** attach child spatial object, setting child's local transform, then update */
        virtual error AttachChild(const SpatialPtr& child, const MathLib::Matrix4& mxChildLocal);
        /** detach child spatial object */
        virtual error DetachChild(const SpatialPtr& child);
        /** get child list */
        virtual const ChildList& GetChildList() { return m_childList; };

        /** @name inner public functions    */
        //@{
        virtual error _UpdateLocalTransform(const MathLib::Matrix4& mxLocal) override;
        virtual error _UpdateWorldData(const MathLib::Matrix4& mxParentWorld) override;
        virtual error _UpdateBoundData() override;

        // notify parent to update me!!
        virtual error _PropagateSpatialRenderState() override;
        // call by parent
        virtual error _UpdateSpatialRenderState() override;
        //@}

        /** enum animator list deep, including geometry's animator */
        //todo : animators
        //virtual void EnumAnimatorListDeep(AnimatorList& resultList) override;

        std::shared_ptr<Node> ThisNode()
        {
            return std::dynamic_pointer_cast<Node, Spatial>(shared_from_this());
        }
        std::shared_ptr<const Node> ThisNode() const
        {
            return std::dynamic_pointer_cast<const Node, const Spatial>(shared_from_this());
        }

    protected:
        //todo : rethink -- mutex for lock list??
        ChildList m_childList;
    };

    using NodePtr = std::shared_ptr<Node>;
};


#endif // NODE_H
