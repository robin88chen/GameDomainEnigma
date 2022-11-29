#include "BoundingVolumeDto.h"
#include "BoundingVolume.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

static std::string TOKEN_BOXBV = "BoxBV";
static std::string TOKEN_SPHEREBV = "SphereBV";

BoundingVolumeDto BoundingVolumeDto::FromGenericDto(const GenericDto& dto)
{
    std::optional<Box3> box;
    std::optional<Sphere3> sphere;
    if (dto.HasValue(TOKEN_BOXBV))
    {
        box = dto.TryGetValue<Box3>(TOKEN_BOXBV);
    }
    else if (dto.HasValue(TOKEN_SPHEREBV))
    {
        sphere = dto.TryGetValue<Sphere3>(TOKEN_SPHEREBV);
    }
    return BoundingVolumeDto{ box, sphere };
}

GenericDto BoundingVolumeDto::ToGenericDto() const
{
    GenericDto dto;
    if (m_box)
    {
        dto.AddOrUpdate(TOKEN_BOXBV, m_box.value());
    }
    else if (m_sphere)
    {
        dto.AddOrUpdate(TOKEN_SPHEREBV, m_sphere.value());
    }
    return dto;
}
