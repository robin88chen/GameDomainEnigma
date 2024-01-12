/*********************************************************************
 * \file   BoundingVolumeDto.h
 * \brief  for serializing
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _BOUNDING_VOLUME_DTO_H
#define _BOUNDING_VOLUME_DTO_H

#include "MathLib/Box3.h"
#include "MathLib/Sphere3.h"
#include "GenericDto.h"
#include <optional>

namespace Enigma::Engine
{
    class BoundingVolumeDto
    {
    public:
        BoundingVolumeDto() = default;
        BoundingVolumeDto(const std::optional<MathLib::Box3>& box, const std::optional<MathLib::Sphere3>& sphere) :
            m_box(box), m_sphere(sphere) {};

        static BoundingVolumeDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto() const;

        std::optional<MathLib::Box3> box() const { return m_box; }
        std::optional<MathLib::Sphere3> sphere() const { return m_sphere; }

    protected:
        std::optional<MathLib::Box3> m_box;
        std::optional<MathLib::Sphere3> m_sphere;
    };
}

#endif // _BOUNDING_VOLUME_DTO_H
