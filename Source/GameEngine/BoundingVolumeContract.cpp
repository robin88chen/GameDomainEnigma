#include "BoundingVolumeContract.h"
#include "BoundingVolume.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

static std::string TOKEN_BOXBV = "BoxBV";
static std::string TOKEN_SPHEREBV = "SphereBV";

BoundingVolumeContract BoundingVolumeContract::FromContract(const Contract& contract)
{
    std::optional<Box3> box;
    std::optional<Sphere3> sphere;
    if (contract.HasValue(TOKEN_BOXBV))
    {
        box = contract.TryGetValue<Box3>(TOKEN_BOXBV);
    }
    else if (contract.HasValue(TOKEN_SPHEREBV))
    {
        sphere = contract.TryGetValue<Sphere3>(TOKEN_SPHEREBV);
    }
    return BoundingVolumeContract{ box, sphere };
}

Contract BoundingVolumeContract::ToContract() const
{
    Contract contract;
    if (m_box)
    {
        contract.AddOrUpdate(TOKEN_BOXBV, m_box.value());
    }
    else if (m_sphere)
    {
        contract.AddOrUpdate(TOKEN_SPHEREBV, m_sphere.value());
    }
    return contract;
}
