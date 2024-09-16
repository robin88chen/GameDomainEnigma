#include "AnimatedPawnAssembler.h"
#include "AnimationClipMapAssembler.h"
#include "AvatarRecipeAssemblers.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "AvatarRecipes.h"
#include "AnimatedPawn.h"
#include "AvatarRecipeQueries.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

static std::string TOKEN_ANIMATION_CLIP_MAP = "AnimClipMap";
static std::string TOKEN_AVATAR_RECIPES = "AvatarRecipes";

AnimatedPawnAssembler::AnimatedPawnAssembler(const SceneGraph::SpatialId& id) : PawnAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI);
}

void AnimatedPawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    PawnAssembler::factory(factory);
    if (m_prefab) m_factoryDesc.ClaimByPrefab(m_prefab.value());
}

void AnimatedPawnAssembler::persist(const std::string& filename, const std::string& path_id)
{
    PawnAssembler::persist(filename, path_id);
    m_prefab.reset();
}

void AnimatedPawnAssembler::animationClipMap(const std::shared_ptr<AnimationClipMapAssembler>& clip)
{
    m_clip = clip;
}

void AnimatedPawnAssembler::animationClipMap(const AnimationClipMap& clip)
{
    m_clip = std::make_shared<AnimationClipMapAssembler>(clip);
}

void AnimatedPawnAssembler::addAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe)
{
    std::shared_ptr<AvatarRecipeAssembler> assembler = recipe->assembler();
    recipe->assemble(assembler);
    addAvatarRecipe(assembler);
}

void AnimatedPawnAssembler::addAvatarRecipe(const std::shared_ptr<AvatarRecipeAssembler>& recipe)
{
    m_avatarRecipes.push_back(recipe);
}

void AnimatedPawnAssembler::byPrefab(const std::string& prefab_name)
{
    m_prefab = prefab_name;
    m_factoryDesc.ClaimByPrefab(prefab_name);
}

Enigma::Engine::GenericDto AnimatedPawnAssembler::assemble() const
{
    Engine::GenericDto dto = PawnAssembler::assemble();
    if (m_clip) dto.addOrUpdate(TOKEN_ANIMATION_CLIP_MAP, m_clip->assemble());
    if (!m_avatarRecipes.empty())
    {
        Engine::GenericDtoCollection avatar_recipe_dtos;
        for (auto& recipe : m_avatarRecipes)
        {
            avatar_recipe_dtos.push_back(recipe->assemble());
        }
        dto.addOrUpdate(TOKEN_AVATAR_RECIPES, avatar_recipe_dtos);
    }
    return dto;
}

AnimatedPawnDisassembler::AnimatedPawnDisassembler() : PawnDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI);
}

void AnimatedPawnDisassembler::disassemble(const Engine::GenericDto& dto)
{
    PawnDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_ANIMATION_CLIP_MAP))
    {
        m_animationClipMap = std::make_shared<AnimationClipMapDisassembler>(v.value())->clipMap();
    }
    m_avatarRecipes.clear();
    if (auto v = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_AVATAR_RECIPES))
    {
        for (auto& recipe_dto : v.value())
        {
            std::shared_ptr<AvatarRecipe> recipe = std::make_shared<RequestAvatarRecipeCreation>(recipe_dto.getRtti().GetRttiName())->dispatch();
            std::shared_ptr<AvatarRecipeDisassembler> disassembler = recipe->disassembler();
            disassembler->disassemble(recipe_dto);
            recipe->disassemble(disassembler);
            m_avatarRecipes.push_back(recipe);
        }
    }
}

/*AnimatedPawnDto AnimatedPawnAssembler::toAnimatedPawnDto()
{
    AnimatedPawnDto pawn_dto(m_pawnAssembler.toGenericDto());
    pawn_dto.id(m_dto.id());
    pawn_dto.animationClipMapDto() = m_dto.animationClipMapDto();
    pawn_dto.factoryDesc(m_dto.factoryDesc());
    return pawn_dto;
}

Enigma::Engine::GenericDto AnimatedPawnAssembler::toGenericDto()
{
    return toAnimatedPawnDto().toGenericDto();
}

std::shared_ptr<AnimatedPawn> AnimatedPawnAssembler::constitute()
{
    return std::dynamic_pointer_cast<AnimatedPawn>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}*/

