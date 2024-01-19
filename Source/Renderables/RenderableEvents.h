/*********************************************************************
 * \file   RenderableEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef RENDERABLE_EVENTS_H
#define RENDERABLE_EVENTS_H

#include "Frameworks/Event.h"
#include "Primitives/Primitive.h"
#include "Animators/AnimationAsset.h"
#include <system_error>

namespace Enigma::Renderables
{
    class ModelPrimitiveAnimator;

    class RenderablePrimitiveBuilt : public Frameworks::IEvent
    {
    public:
        RenderablePrimitiveBuilt(const Primitives::PrimitiveId& id, const std::shared_ptr<Primitives::Primitive> prim) : m_id(id), m_prim(prim) {};

        const Primitives::PrimitiveId& id() { return m_id; }
        const std::shared_ptr<Primitives::Primitive>& primitive() { return m_prim; }

    private:
        Primitives::PrimitiveId m_id;
        std::shared_ptr<Primitives::Primitive> m_prim;
    };
    class BuildRenderablePrimitiveFailed : public Frameworks::IEvent
    {
    public:
        BuildRenderablePrimitiveFailed(const Primitives::PrimitiveId& id, std::error_code er) : m_id(id), m_error(er) {};

        const Primitives::PrimitiveId& id() { return m_id; }
        std::error_code error() const { return m_error; }

    private:
        Primitives::PrimitiveId m_id;
        std::error_code m_error;
    };

    class ModelAnimatorBuilt : public Frameworks::IEvent
    {
    public:
        ModelAnimatorBuilt(const Frameworks::Ruid& ruid, const std::shared_ptr<ModelPrimitiveAnimator>& ani) :
            m_ruid(ruid), m_animator(ani) {};
        const Frameworks::Ruid& getRuid() { return m_ruid; }
        const std::shared_ptr<ModelPrimitiveAnimator>& getAnimator() { return m_animator; }
    private:
        Frameworks::Ruid m_ruid;
        std::shared_ptr<ModelPrimitiveAnimator> m_animator;
    };
    class BuildModelAnimatorFailed : public Frameworks::IEvent
    {
    public:
        BuildModelAnimatorFailed(const Frameworks::Ruid& ruid, std::error_code er) :
            m_ruid(ruid), m_error(er) {};
        const Frameworks::Ruid& getRuid() { return m_ruid; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        Frameworks::Ruid m_ruid;
        std::error_code m_error;
    };
    class AnimationAssetBuilt : public Frameworks::IEvent
    {
    public:
        AnimationAssetBuilt(const std::string& name, const std::shared_ptr<Animators::AnimationAsset>& ani) :
            m_name(name), m_animation(ani) {};
        const std::string& getName() { return m_name; }
        const std::shared_ptr<Animators::AnimationAsset>& GetAnimationAsset() { return m_animation; }
    private:
        std::string m_name;
        std::shared_ptr<Animators::AnimationAsset> m_animation;
    };
    class BuildAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        BuildAnimationAssetFailed(const std::string& name, std::error_code er) :
            m_name(name), m_error(er) {};
        const std::string& getName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class FactoryAnimationAssetCreated : public Frameworks::IEvent
    {
    public:
        FactoryAnimationAssetCreated(const Frameworks::Ruid& ruid, const std::shared_ptr<Animators::AnimationAsset>& ani)
            : m_ruid(ruid), m_animation(ani) {};

        const Frameworks::Ruid& GetConstructingRuid() const { return m_ruid; }
        const std::shared_ptr<Animators::AnimationAsset>& GetAnimationAsset() { return m_animation; }

    protected:
        Frameworks::Ruid m_ruid;
        std::shared_ptr<Animators::AnimationAsset> m_animation;
    };
}

#endif // RENDERABLE_EVENTS_H
