/*********************************************************************
 * \file   AvatarRecipies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_RECIPES_H
#define _AVATAR_RECIPES_H

#include "SceneGraph/Pawn.h"
#include "GameEngine/EffectMaterialDto.h"
#include "Renderer/MeshPrimitive.h"

namespace Enigma::GameCommon
{
    class AvatarRecipe
    {
    public:
        AvatarRecipe() = default;
        virtual ~AvatarRecipe() = default;

        virtual void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) = 0;
    };

    class ReplaceAvatarMaterial : public AvatarRecipe
    {
    public:
        ReplaceAvatarMaterial(const std::string& old_material_name, const Engine::EffectMaterialDto& new_material_dto);
        virtual ~ReplaceAvatarMaterial() override;

        void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;

    private:
        void ReplaceMeshMaterial(const Renderer::MeshPrimitivePtr& mesh);

        void OnCompileEffectResponse(const Frameworks::IResponsePtr& r);

    private:
        std::string m_oldMaterialName;
        Engine::EffectMaterialDto m_newMaterialDto;

        Frameworks::ResponseSubscriberPtr m_onCompileEffectResponse;
        using ChangeSpecifyMaterial = std::function<void(const std::shared_ptr<Engine::EffectMaterial>&)>;
        std::unordered_map<Frameworks::Ruid, ChangeSpecifyMaterial, Frameworks::Ruid::HashFunc> m_changeSpecifyMaterialMap;
    };
}

#endif // _AVATAR_RECIPES_H
