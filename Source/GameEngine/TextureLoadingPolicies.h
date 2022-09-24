/*********************************************************************
 * \file   TextureLoadingPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_LOADING_POLICIES_H
#define TEXTURE_LOADING_POLICIES_H

#include "FileSystem/Filename.h"
#include <string>

namespace Enigma::Engine
{
	struct TexturePolicy
	{
        std::string m_name;
        std::string m_filename;
        std::string m_pathId;
        TexturePolicy() {};
        TexturePolicy(const std::string& name, const std::string& filename, const std::string& path_id)
            : m_name(name), m_filename(filename), m_pathId(path_id) {};
        TexturePolicy(const std::string& name, const FileSystem::Filename& filename) : m_name(name)
        {
            m_filename = filename.GetFileName();
            m_pathId = filename.GetMountPathID();
        };
	};
}

#endif // TEXTURE_LOADING_POLICIES_H
