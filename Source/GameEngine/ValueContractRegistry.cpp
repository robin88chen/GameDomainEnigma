#include "ValueContractRegistry.h"
#include "BoundingVolume.h"
#include "BoundingVolumeContract.h"
#include "Frameworks/ContractedFactories.h"
#include <any>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

std::any BoundingVolumeFactory(const Contract& contract)
{
    return BoundingVolume(BoundingVolumeContract::FromContract(contract));
}

void ValueContractRegistry::RegisterValueFactories()
{
    ContractedFactories::RegisterFactory(BoundingVolume::TYPE_RTTI.GetName(), BoundingVolumeFactory);
}

void ValueContractRegistry::UnregisterValueFactories()
{
    ContractedFactories::UnregisterFactory(BoundingVolume::TYPE_RTTI.GetName());
}
