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
#include "GameEngine/EffectTextureMapDto.h"
#include "Renderer/MeshPrimitive.h"

namespace Enigma::GameCommon
{
    class AvatarRecipe
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        AvatarRecipe() = default;
        virtual ~AvatarRecipe() = default;

        virtual void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) = 0;
        virtual Engine::GenericDto ToGenericDto() const = 0;

        static std::shared_ptr<AvatarRecipe> CreateFromGenericDto(const Engine::GenericDto& dto);
    };

    class ReplaceAvatarMaterial : public AvatarRecipe
    {
        DECLARE_EN_RTTI;
    public:
        ReplaceAvatarMaterial(const std::string& old_material_name, const Engine::EffectMaterialDto& new_material_dto);
        ReplaceAvatarMaterial(const Engine::GenericDto& o);
        virtual ~ReplaceAvatarMaterial() override;

        void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto ToGenericDto() const override;

    private:
        void ReplaceMeshMaterial(const Renderer::MeshPrimitivePtr& mesh);

        void OnCompileEffectResponse(const Frameworks::IResponsePtr& r);

    private:
        std::weak_ptr<Engine::Primitive> m_primitive;
        std::string m_oldMaterialName;
        Engine::EffectMaterialDto m_newMaterialDto;

        Frameworks::ResponseSubscriberPtr m_onCompileEffectResponse;
        using ChangeSpecifyMaterial = std::function<void(const std::shared_ptr<Engine::EffectMaterial>&)>;
        std::unordered_map<Frameworks::Ruid, ChangeSpecifyMaterial, Frameworks::Ruid::HashFunc> m_changeSpecifyMaterialMap;
    };
    class ChangeAvatarTexture : public AvatarRecipe
    {
        DECLARE_EN_RTTI;
    public:
        ChangeAvatarTexture(const std::string& mesh_name, const Engine::TextureMappingDto& texture_dto);
        ChangeAvatarTexture(const Engine::GenericDto& o);
        virtual ~ChangeAvatarTexture() override;
        void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto ToGenericDto() const override;

    private:
        void ChangeMeshTexture(const Renderer::MeshPrimitivePtr& mesh);
        void OnLoadTextureResponse(const Frameworks::IResponsePtr& r);

    private:
        std::string m_meshName;
        Engine::TextureMappingDto m_textureDto;
        Frameworks::ResponseSubscriberPtr m_onLoadTextureResponse;
        std::weak_ptr<Renderer::MeshPrimitive> m_mesh;
        Frameworks::Ruid m_requsetRuid;
    };
}

#endif // _AVATAR_RECIPES_H
