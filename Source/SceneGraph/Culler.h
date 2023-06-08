/*********************************************************************
 * \file   Culler.h
 * \brief  culler class, value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef CULLER_H
#define CULLER_H

#include "GameEngine/BoundingVolume.h"
#include "VisibleSet.h"
#include <memory>
#include <system_error>
#include <bitset>

namespace Enigma::SceneGraph
{
    using error = std::error_code;
    class Camera;
    class VisibleSet;
    class Spatial;

    /** Scene Graph Culler object */
    class Culler
    {
    public:
        enum class CullerPlane  // clip plane 陣列index
        {
            Left = 0,
            Right,
            Top,
            Bottom,
            Back,
            Front,
            Count,
        };
        enum { CULLER_MAX_PLANE_QUANTITY = 32 };
        using PlaneActivationBits = std::bitset<CULLER_MAX_PLANE_QUANTITY>;

    public:
        Culler(const std::shared_ptr<Camera>& camera);
        Culler(const Culler& culler);
        Culler(Culler&&) noexcept;
        virtual ~Culler();
        Culler& operator=(const Culler& culler);
        Culler& operator=(Culler&&) noexcept;

        void SetCamera(const std::shared_ptr<Camera>& camera);
        const std::shared_ptr<Camera>& GetCamera() const { return m_camera; };

        const VisibleSet& GetVisibleSet() const { return m_visibleSet; };

        error ComputeVisibleSet(const std::shared_ptr<Spatial>& scene);

        bool IsVisible(const Engine::BoundingVolume& bound);
        bool IsVisible(MathLib::Vector3* vecPos, unsigned int quantity, bool isIgnoreNearPlane);
        void EnableOuterClipping(bool flag) { m_isEnableOuterClipping = flag; };
        bool IsOuterClippingEnable() { return m_isEnableOuterClipping; };
        bool IsOutVisibility(const Engine::BoundingVolume& bound);

        virtual void Insert(const std::shared_ptr<Spatial>& obj);

        unsigned int GetPlaneQuantity() const { return m_countCullerPlane; };
        const std::vector<MathLib::Plane3>& GetPlanes() const { return m_clipPlanes; };

        void UpdateFrustumPlanes();

        PlaneActivationBits GetPlaneActivations() { return m_planeActivations; };
        void RestorePlaneBitFlags(PlaneActivationBits flags) { m_planeActivations = flags; };

        void PushAdditionalPlane(const MathLib::Plane3& plane);
        void RemoveAdditionalPlane();

    protected:

        std::shared_ptr<Camera> m_camera;

        /** @remark The world culling planes corresponding to the view frustum plus any
        additional user-defined culling planes. \n
        The member m_uiPlaneBitFlags represents bit flags to store whether or not a plane is active in the
        culling system.\n
        A bit of 1 means the plane is active, otherwise the
        plane is inactive. \n  An active plane is compared to bounding volumes,
        whereas an inactive plane is not. \n This supports an efficient culling
        of a hierarchy. \n  For example, if a node's bounding volume is inside
        the left plane of the view frustum, then the left plane is set to
        inactive because the children of the node are automatically all inside
        the left plane. */
        PlaneActivationBits m_planeActivations;

        unsigned int m_countCullerPlane;
        std::vector<MathLib::Plane3> m_clipPlanes;

        bool m_isEnableOuterClipping; ///< default is false
        /// for calculating un-visible spatial
        std::vector<MathLib::Plane3> m_outerClipPlanes;
        /// 將 clip-plane 的原點往後挪，frustum 就會相對比較大, default = 2.0f
        float m_outerClipShiftZ;

        VisibleSet m_visibleSet;
    };
};


#endif // CULLER_H
