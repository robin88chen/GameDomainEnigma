#include "AvatarRecipes.h"
#include "AvatarBaker.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

void ReplaceAvatarMaterial::Bake(const std::shared_ptr<Pawn>& pawn)
{
    AvatarBaker::ReplaceAvatarMaterial(pawn, m_oldMaterialName, m_newMaterialDto);
}

