/*********************************************************************
 * \file   SphereBV.h
 * \brief  Sphere Bounding volume, value object, 用 std::unique_ptr 操作
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SPHERE_BV_H
#define SPHERE_BV_H

#include "GenericBV.h"
#include "MathLib/Sphere3.h"

namespace Enigma::Engine
{
    /** sphere bounding volume */
    class SphereBV : public GenericBV
    {
    public:
        SphereBV(const MathLib::Sphere3&);
        SphereBV(const SphereBV&) = default;
        SphereBV(SphereBV&&) = default;
        virtual ~SphereBV() override;
        SphereBV& operator=(const SphereBV&) = default;
        SphereBV& operator=(SphereBV&&) = default;

        virtual void CreateFromTransform(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source) override;
        virtual void Copy(const std::unique_ptr<GenericBV>& source) override;
        virtual void ZeroReset() override;
        virtual void MergeBoundingVolume(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source) override;
        virtual MathLib::Plane3::SideOfPlane WhichSide(const MathLib::Plane3& plane) const override;

        virtual void ComputeFromData(const MathLib::Vector3* pos, unsigned int quantity, bool axis_align) override;
        virtual void ComputeFromData(const MathLib::Vector4* pos, unsigned int quantity, bool axis_align) override;
        virtual void ComputeFromData(const float* vert, unsigned int pitch, unsigned int quantity,
            bool axis_align) override;

        virtual const MathLib::Vector3& GetCenterPos() override { return m_sphere.Center(); };
        virtual bool PointInside(const MathLib::Vector3& vecPos) override;

        virtual bool IsEmpty() const override { return m_sphere.IsEmpty(); };

        MathLib::Sphere3& GetSphere() { return m_sphere; };

    private:
        MathLib::Sphere3 m_sphere;
    };
};


#endif // SPHERE_BV_H
