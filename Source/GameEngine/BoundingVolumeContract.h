/*********************************************************************
 * \file   BoundingVolumeContract.h
 * \brief  for serializing
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _BOUNDING_VOLUME_CONTRACT_H
#define _BOUNDING_VOLUME_CONTRACT_H

#include "MathLib/Box3.h"
#include "MathLib/Sphere3.h"
#include "Frameworks/Contract.h"
#include <optional>

namespace Enigma::Engine
{
    class BoundingVolumeContract
    {
    public:
        BoundingVolumeContract() = default;
        BoundingVolumeContract(const std::optional<MathLib::Box3>& box, const std::optional<MathLib::Sphere3>& sphere) :
            m_box(box), m_sphere(sphere) {};

        static BoundingVolumeContract FromContract(const Frameworks::Contract& contract);
        Frameworks::Contract ToContract() const;

        std::optional<MathLib::Box3> Box() const { return m_box; }
        std::optional<MathLib::Sphere3> Sphere() const { return m_sphere; }

    protected:
        std::optional<MathLib::Box3> m_box;
        std::optional<MathLib::Sphere3> m_sphere;
    };
}

#endif // _BOUNDING_VOLUME_CONTRACT_H
