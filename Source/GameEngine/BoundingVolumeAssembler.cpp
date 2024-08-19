#include "BoundingVolumeAssembler.h"
#include "BoundingVolume.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

static std::string TOKEN_BOXBV = "BoxBV";
static std::string TOKEN_SPHEREBV = "SphereBV";

GenericDto BoundingVolumeAssembler::assemble() const
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

BoundingVolumeDisassembler::BoundingVolumeDisassembler(const GenericDto& dto)
{
    if (dto.hasValue(TOKEN_BOXBV))
    {
        m_box = dto.tryGetValue<Box3>(TOKEN_BOXBV);
    }
    else if (dto.hasValue(TOKEN_SPHEREBV))
    {
        m_sphere = dto.tryGetValue<Sphere3>(TOKEN_SPHEREBV);
    }
}
