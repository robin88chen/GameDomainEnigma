/*********************************************************************
 * \file   RenderableCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef RENDERABLE_COMMANDS_H
#define RENDERABLE_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Renderables
{
    class AnimationAssetPolicy;
    class ModelAnimatorPolicy;

    class BuildRenderablePrimitive : public Frameworks::IRequestCommand
    {
    public:
        BuildRenderablePrimitive(const Engine::GenericDto& primitive_dto) : m_primitiveDto(primitive_dto) {}
        const Engine::GenericDto& primitiveDto() { return m_primitiveDto; }

    private:
        Engine::GenericDto m_primitiveDto;
    };

    class BuildAnimationAsset : public Frameworks::ICommand
    {
    public:
        BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<AnimationAssetPolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<AnimationAssetPolicy> m_policy;
    };

    class BuildModelAnimator : public Frameworks::ICommand
    {
    public:
        BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<ModelAnimatorPolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<ModelAnimatorPolicy> m_policy;
    };
}

#endif // RENDERABLE_COMMANDS_H
