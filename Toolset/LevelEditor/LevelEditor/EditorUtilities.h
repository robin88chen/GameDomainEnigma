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
#include <nana/gui/widgets/treebox.hpp>
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/ColorRGBA.h"
#include "SceneGraph/SpatialId.h"
#include "WorldMap/WorldMapId.h"

namespace LevelEditor
{
    void pasteTextureImageToButton(const std::string& filepath, nana::button* btn, const unsigned int btn_size);
    std::string filePathCombinePathID(const std::filesystem::path& path, const std::string& path_id);
    std::tuple<Enigma::MathLib::Vector3, bool> parseTextToVector3(const std::string& value);
    std::tuple<Enigma::MathLib::Vector2, bool> parseTextToVector2(const std::string& value);
    std::tuple<Enigma::MathLib::ColorRGBA, bool> parseTextToColorRGBA(const std::string& value);

    double getSystemTime();

    bool clearTreeItemValue(nana::treebox::item_proxy i);

    std::string idToTreeViewKey(const Enigma::SceneGraph::SpatialId& id);
    std::string idToTreeViewKey(const Enigma::WorldMap::WorldMapId& id);
}


#endif // EDITOR_UTILITIES_H
