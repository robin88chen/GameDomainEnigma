/*********************************************************************
 * \file   GenericBV.h
 * \brief  Generic Bounding Volume value object,
 *  因為有是繼承出多形類別，需要用指標來操作，限定使用 unique_ptr
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef GENERIC_BV_H
#define GENERIC_BV_H

#include "MathLib/Matrix4.h"
#include "MathLib/Plane3.h"
#include "MathLib/Vector3.h"
#include <memory>

namespace Enigma::Engine
{
    /** Bounding Volume abstract object */
    class GenericBV
    {
    public:
        GenericBV();
        GenericBV(const GenericBV&) = default;
        GenericBV(GenericBV&&) = default;
        virtual ~GenericBV();
        GenericBV& operator=(const GenericBV&) = default;
        GenericBV& operator=(GenericBV&&) = default;

        /** create bounding volume from source transformed by matrix, << 需要實作的虛擬函式不能掛 static >> */
        virtual void CreateFromTransform(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source) = 0;
        /** copy bounding volume */
        virtual void Copy(const std::unique_ptr<GenericBV>& source) = 0;
        /** reset to zero bound */
        virtual void ZeroReset() = 0;
        /** merge with other bounding volume */
        //todo : 引用的計算，必須要是不為空的BV
        virtual void MergeBoundingVolume(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source) = 0;

        /** which side of the plane? */
        virtual MathLib::Plane3::SideOfPlane WhichSide(const MathLib::Plane3& plane) const = 0;

        /** compute from data, with vector3 array
        @param pos position array
        @param quantity data quantity
        @param axis_align axis-aligned or oriented
        */
        virtual void ComputeFromData(const MathLib::Vector3* pos, unsigned int quantity, bool axis_align) = 0;
        /** compute from data, with vector4 array
        @param pos position array
        @param quantity data quantity
        */
        virtual void ComputeFromData(const MathLib::Vector4* pos, unsigned int quantity, bool axis_align) = 0;
        /** compute from data, with vertex array, pitch means how many float datas in a vertex
        @param vert vertex array
        @param pitch =sizeof(vertex)/sizeof(float)
        @param quantity data quantity
        */
        virtual void ComputeFromData(const float* vert, unsigned int pitch, unsigned int quantity, bool axis_align) = 0;

        /** get center pos */
        virtual const MathLib::Vector3& GetCenterPos() = 0;

        /** is inside */
        virtual bool PointInside(const MathLib::Vector3& pos) = 0;

        virtual bool IsEmpty() const = 0;
    };
};


#endif // GENERIC_BV_H
