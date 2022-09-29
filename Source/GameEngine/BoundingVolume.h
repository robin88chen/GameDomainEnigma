/*********************************************************************
 * \file   BoundingVolume.h
 * \brief  Bounding Volume, value object, (Generic BV wrap obj)
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include "MathLib/Box3.h"
#include "MathLib/Matrix4.h"
#include "MathLib/Sphere3.h"
#include "MathLib/Plane3.h"
#include <optional>
#include <memory>

namespace Enigma::Engine
{
    class GenericBV;
    class BoundingVolume
    {
    public:
        enum class Side
        {
            Failed,
            Positive,
            Negative,
            Overlap,
        };
    public:
        BoundingVolume();
        BoundingVolume(const MathLib::Box3& box);
        BoundingVolume(const MathLib::Sphere3& sphere);
        BoundingVolume(const BoundingVolume&);
        BoundingVolume(BoundingVolume&&);
        ~BoundingVolume();

        BoundingVolume& operator=(const BoundingVolume&);
        BoundingVolume& operator=(BoundingVolume&&);

        static BoundingVolume CreateFromTransform(const BoundingVolume& source_bv, const MathLib::Matrix4& mx);
        void Merge(const MathLib::Matrix4& to_mx, const BoundingVolume& to_bv);

        bool IsEmpty() const;

        std::optional<MathLib::Box3> BoundingBox3() const;
        std::optional<MathLib::Sphere3> BoundingSphere3() const;

        bool PointInside(const MathLib::Vector3& pos) const;
        Side SideOfPlane(const MathLib::Plane3& plane) const;

    private:
        std::unique_ptr<GenericBV> m_bv;
    };
}


#endif // BOUNDING_VOLUME_H
