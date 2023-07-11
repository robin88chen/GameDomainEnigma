/*********************************************************************
 * \file   EditorUtilities.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef EDITOR_UTILITIES_H
#define EDITOR_UTILITIES_H

#include <string>
#include <nana/gui/widgets/button.hpp>
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"

namespace LevelEditor
{
    void PasteTextureImageToButton(const std::string& filepath, nana::button* btn, const unsigned int btn_size);
    std::string ImagePathCombinePathID(const std::filesystem::path& path);
    std::tuple<Enigma::MathLib::Vector3, bool> ParseTextToVector3(const std::string& value);
    std::tuple<Enigma::MathLib::Vector2, bool> ParseTextToVector2(const std::string& value);
}


#endif // EDITOR_UTILITIES_H
