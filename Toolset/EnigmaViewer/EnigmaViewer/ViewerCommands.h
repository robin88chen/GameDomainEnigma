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
#include "Renderer/ModelPrimitive.h"
#include "Animators/AnimationClip.h"
#include "GameCommon/AnimationClipMap.h"
#include <string>

namespace EnigmaViewer
{
    class OutputMessage : public Enigma::Frameworks::ICommand
    {
    public:
        OutputMessage(const std::string& msg) : m_msg(msg) {}

        const std::string& GetMessage() const { return m_msg; }

    private:
        std::string m_msg;
    };
    class RefreshModelNodeTree : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshModelNodeTree(const std::shared_ptr<Enigma::Renderer::ModelPrimitive>& model) : m_model(model) {}

        const std::shared_ptr<Enigma::Renderer::ModelPrimitive>& GetModel() const { return m_model; }
    private:
        std::shared_ptr<Enigma::Renderer::ModelPrimitive> m_model;
    };
    class RefreshAnimationClipList : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshAnimationClipList(const Enigma::GameCommon::AnimationClipMap& clip_map) : m_clipMap(clip_map) {}

        const Enigma::GameCommon::AnimationClipMap& GetClipMap() const { return m_clipMap; }

    private:
        Enigma::GameCommon::AnimationClipMap m_clipMap;
    };
    class ChangeMeshTexture : public Enigma::Frameworks::ICommand
    {
    public:
        ChangeMeshTexture(const std::string& mesh_name, const std::string& tex_filename) : m_meshName(mesh_name), m_texFilename(tex_filename) {};

        const std::string& GetMeshName() const { return m_meshName; }
        const std::string& GetTextureFilename() const { return m_texFilename; }

    private:
        std::string m_meshName;
        std::string m_texFilename;
    };
    class AddAnimationClip : public Enigma::Frameworks::ICommand
    {
    public:
        AddAnimationClip(const std::string& name, const Enigma::Animators::AnimationClip& clip) : m_name(name), m_clip(clip) {}
        const std::string& GetName() const { return m_name; }
        const Enigma::Animators::AnimationClip& GetClip() const { return m_clip; }

    private:
        std::string m_name;
        Enigma::Animators::AnimationClip m_clip;
    };
    class DeleteAnimationClip : public Enigma::Frameworks::ICommand
    {
    public:
        DeleteAnimationClip(const std::string& name) : m_name(name) {}
        const std::string& GetName() const { return m_name; }

    private:
        std::string m_name;
    };
    class PlayAnimationClip : public Enigma::Frameworks::ICommand
    {
    public:
        PlayAnimationClip(const std::string& name) : m_name(name) {}
        const std::string& GetName() const { return m_name; }

    private:
        std::string m_name;
    };
    class ChangeAnimationTimeValue : public Enigma::Frameworks::ICommand
    {
    public:
        ChangeAnimationTimeValue(const std::string& old_name, const std::string& new_name, const Enigma::Animators::AnimationClip& clip)
            : m_oldName(old_name), m_newName(new_name), m_clip(clip) {}
        const std::string& GetOldName() const { return m_oldName; }
        const std::string& GetNewName() const { return m_newName; }
        const Enigma::Animators::AnimationClip& GetClip() const { return m_clip; }

    private:
        std::string m_oldName;
        std::string m_newName;
        Enigma::Animators::AnimationClip m_clip;
    };
}
#endif // _VIEWER_COMMANDS_H
