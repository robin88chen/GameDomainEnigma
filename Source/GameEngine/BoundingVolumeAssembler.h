/*********************************************************************
 * \file   BoundingVolumeAssembler.h
 * \brief
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
    class BoundingVolumeAssembler
    {
    public:
        BoundingVolumeAssembler() = default;

        void box(const MathLib::Box3& box) { m_box = box; }
        void sphere(const MathLib::Sphere3& sphere) { m_sphere = sphere; }

        GenericDto assemble() const;

    protected:
        std::optional<MathLib::Box3> m_box;
        std::optional<MathLib::Sphere3> m_sphere;
    };

    class BoundingVolumeDisassembler
    {
    public:
        BoundingVolumeDisassembler(const GenericDto& dto);

        [[nodiscard]] const std::optional<MathLib::Box3>& box() const { return m_box; }
        [[nodiscard]] const std::optional<MathLib::Sphere3>& sphere() const { return m_sphere; }

    protected:
        std::optional<MathLib::Box3> m_box;
        std::optional<MathLib::Sphere3> m_sphere;
    };
}

#endif // _BOUNDING_VOLUME_DTO_H
