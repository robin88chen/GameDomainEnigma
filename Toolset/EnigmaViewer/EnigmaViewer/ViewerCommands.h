/*********************************************************************
 * \file   ViewerCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _VIEWER_COMMANDS_H
#define _VIEWER_COMMANDS_H

#include "Frameworks/Command.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/AnimationClip.h"
#include "GameCommon/AnimationClipMap.h"
#include "GameEngine/TextureId.h"
#include <string>

namespace EnigmaViewer
{
    class OutputMessage : public Enigma::Frameworks::ICommand
    {
    public:
        OutputMessage(const std::string& msg) : m_msg(msg) {}

        const std::string& message() const { return m_msg; }

    private:
        std::string m_msg;
    };
    class RefreshModelNodeTree : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshModelNodeTree(const std::shared_ptr<Enigma::Renderables::ModelPrimitive>& model) : m_model(model) {}

        const std::shared_ptr<Enigma::Renderables::ModelPrimitive>& model() const { return m_model; }
    private:
        std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    };
    class RefreshAnimationClipList : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshAnimationClipList(const Enigma::GameCommon::AnimationClipMap& clip_map) : m_clipMap(clip_map) {}

        const Enigma::GameCommon::AnimationClipMap& clipMap() const { return m_clipMap; }

    private:
        Enigma::GameCommon::AnimationClipMap m_clipMap;
    };
    class ChangeMeshTexture : public Enigma::Frameworks::ICommand
    {
    public:
        ChangeMeshTexture(const std::string& mesh_name, const Enigma::Engine::TextureId& tex_id) : m_meshName(mesh_name), m_texId(tex_id) {};

        const std::string& meshName() const { return m_meshName; }
        const Enigma::Engine::TextureId& textureId() const { return m_texId; }

    private:
        std::string m_meshName;
        Enigma::Engine::TextureId m_texId;
    };
    class AddAnimationClip : public Enigma::Frameworks::ICommand
    {
    public:
        AddAnimationClip(const std::string& name, const Enigma::Renderables::AnimationClip& clip) : m_name(name), m_clip(clip) {}
        const std::string& name() const { return m_name; }
        const Enigma::Renderables::AnimationClip& clip() const { return m_clip; }

    private:
        std::string m_name;
        Enigma::Renderables::AnimationClip m_clip;
    };
    class DeleteAnimationClip : public Enigma::Frameworks::ICommand
    {
    public:
        DeleteAnimationClip(const std::string& name) : m_name(name) {}
        const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };
    class PlayAnimationClip : public Enigma::Frameworks::ICommand
    {
    public:
        PlayAnimationClip(const std::string& name) : m_name(name) {}
        const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };
    class ChangeAnimationTimeValue : public Enigma::Frameworks::ICommand
    {
    public:
        ChangeAnimationTimeValue(const std::string& old_name, const std::string& new_name, const Enigma::Renderables::AnimationClip& clip)
            : m_oldName(old_name), m_newName(new_name), m_clip(clip) {}
        const std::string& oldName() const { return m_oldName; }
        const std::string& newName() const { return m_newName; }
        const Enigma::Renderables::AnimationClip& clip() const { return m_clip; }

    private:
        std::string m_oldName;
        std::string m_newName;
        Enigma::Renderables::AnimationClip m_clip;
    };
    class RefreshModelPrimitiveList : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshModelPrimitiveList(const std::vector<std::string>& model_list) : m_modelList(model_list) {}

        const std::vector<std::string>& modelList() const { return m_modelList; }

    private:
        std::vector<std::string> m_modelList;
    };
    class LoadModelPrimitive : public Enigma::Frameworks::ICommand
    {
    public:
        LoadModelPrimitive(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };
    class RemoveModelPrimitive : public Enigma::Frameworks::ICommand
    {
    public:
        RemoveModelPrimitive(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };
    class CreateAnimatedPawn : public Enigma::Frameworks::ICommand
    {
    public:
        CreateAnimatedPawn(const std::string& name, const Enigma::Primitives::PrimitiveId& model_id) : m_name(name), m_modelId(model_id) {}

        const std::string& name() const { return m_name; }
        const Enigma::Primitives::PrimitiveId& modelId() const { return m_modelId; }

    protected:
        std::string m_name;
        Enigma::Primitives::PrimitiveId m_modelId;
    };
    class RefreshPawnList : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshPawnList(const std::vector<std::string>& pawn_list) : m_pawnList{ pawn_list } {}

        const std::vector<std::string>& pawnList() const { return m_pawnList; }

    private:
        std::vector<std::string> m_pawnList;
    };
    class LoadAnimatedPawn : public Enigma::Frameworks::ICommand
    {
    public:
        LoadAnimatedPawn(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };
    class RemoveAnimatedPawn : public Enigma::Frameworks::ICommand
    {
    public:
        RemoveAnimatedPawn(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

    private:
        std::string m_name;
    };
}

#endif // _VIEWER_COMMANDS_H
