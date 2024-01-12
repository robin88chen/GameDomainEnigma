#include "BoundingVolumeDto.h"
#include "BoundingVolume.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

static std::string TOKEN_BOXBV = "BoxBV";
static std::string TOKEN_SPHEREBV = "SphereBV";

BoundingVolumeDto BoundingVolumeDto::fromGenericDto(const GenericDto& dto)
{
    std::optional<Box3> box;
    std::optional<Sphere3> sphere;
    if (dto.hasValue(TOKEN_BOXBV))
    {
        box = dto.tryGetValue<Box3>(TOKEN_BOXBV);
    }
    else if (dto.hasValue(TOKEN_SPHEREBV))
    {
        sphere = dto.tryGetValue<Sphere3>(TOKEN_SPHEREBV);
    }
    return BoundingVolumeDto{ box, sphere };
}

GenericDto BoundingVolumeDto::toGenericDto() const
{
    GenericDto dto;
    if (m_box)
    {
        dto.addOrUpdate(TOKEN_BOXBV, m_box.value());
    }
    else if (m_sphere)
    {
        dto.addOrUpdate(TOKEN_SPHEREBV, m_sphere.value());
    }
    return dto;
}
