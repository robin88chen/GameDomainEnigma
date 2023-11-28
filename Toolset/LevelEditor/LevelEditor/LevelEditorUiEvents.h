/*********************************************************************
 * \file   LevelEditorUiEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LEVEL_EDITOR_UI_EVENTS_H
#define LEVEL_EDITOR_UI_EVENTS_H

#include "Frameworks/Event.h"
namespace LevelEditor
{
    class TerrainBrushSizeChanged : public Enigma::Frameworks::IEvent
    {
    public:
        TerrainBrushSizeChanged(unsigned size) : m_size(size) {}

        unsigned size() const { return m_size; }

    private:
        unsigned m_size;
    };
    class TerrainBrushHeightChanged : public Enigma::Frameworks::IEvent
    {
    public:
        TerrainBrushHeightChanged(float height) : m_height(height) {}

        float height() const { return m_height; }

    private:
        float m_height;
    };
    class TerrainBrushDensityChanged : public Enigma::Frameworks::IEvent
    {
    public:
        TerrainBrushDensityChanged(float density) : m_density(density) {}

        float density() const { return m_density; }

    private:
        float m_density;
    };
    class TerrainPaintingLayerChanged : public Enigma::Frameworks::IEvent
    {
    public:
        TerrainPaintingLayerChanged(unsigned layer) : m_layer(layer) {}

        unsigned layer() const { return m_layer; }

    private:
        unsigned m_layer;
    };
    class TerrainEditToolSelected : public Enigma::Frameworks::IEvent
    {
    public:
        enum class Tool
        {
            Raise,
            Lower,
            Paint
        };
    public:
        TerrainEditToolSelected(Tool tool) : m_tool(tool) {}

        Tool tool() const { return m_tool; }
    private:
        Tool m_tool;
    };
}

#endif // LEVEL_EDITOR_UI_EVENTS_H
