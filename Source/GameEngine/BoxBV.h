/*********************************************************************
 * \file   BoxBV.h
 * \brief  Box Bounding Volume value object, 用 unique_ptr 操作
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef BOX_BV_H
#define BOX_BV_H

#include "GenericBV.h"
#include "MathLib/Box3.h"
#include <bitset>

namespace Enigma::Engine
{
    /** Box Bounding Volume */
    class BoxBV : public GenericBV
    {
    public:
        enum class Axis { x, y, z, count  };
        using FlagBits = std::bitset<(size_t)Axis::count>;

    public:
        BoxBV(const MathLib::Box3&);
        BoxBV(const BoxBV&) = default;
        BoxBV(BoxBV&&) = default;
        virtual ~BoxBV();
        BoxBV& operator=(const BoxBV&) = default;
        BoxBV& operator=(BoxBV&&) = default;

        virtual void CreateFromTransform(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source) override;
        virtual void Copy(const std::unique_ptr<GenericBV>& source) override;
        virtual void ZeroReset() override;
        virtual void MergeBoundingVolume(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source) override;
        virtual MathLib::Plane3::SideOfPlane WhichSide(const MathLib::Plane3& plane) const override;

        virtual void ComputeFromData(const MathLib::Vector3* pos, unsigned int quantity, bool axis_align) override;
        virtual void ComputeFromData(const MathLib::Vector4* pos, unsigned int quantity, bool axis_align) override;
        virtual void ComputeFromData(const float* vert, unsigned int pitch,
            unsigned int quantity, bool axis_align) override;

        virtual const MathLib::Vector3& GetCenterPos() override { return m_box.Center(); };
        virtual bool PointInside(const MathLib::Vector3& pos) override;

        virtual bool IsEmpty() const override { return m_box.IsEmpty(); };
        /** point inside flags
        @remark BoxBV使用的函式。\n
        計算點位置是否在三個軸內，0x01, 0x02, 0x04 表示 0,1,2三個軸之內 \n
        用途是，如果不考慮y方向的位置的話，就將傳回值做 0x05的 and 檢查就可以了
        */
        virtual FlagBits PointInsideFlags(const MathLib::Vector3& pos);

        MathLib::Box3& GetBox() { return m_box; };

    private:
        MathLib::Box3 m_box;
    };
};


#endif // BOX_BV_H
