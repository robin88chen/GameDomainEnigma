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

        unsigned GetSize() const { return m_size; }

    private:
        unsigned m_size;
    };
    class TerrainBrushHeightChanged : public Enigma::Frameworks::IEvent
    {
    public:
        TerrainBrushHeightChanged(float height) : m_height(height) {}

        float GetHeight() const { return m_height; }

    private:
        float m_height;
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

        Tool GetTool() const { return m_tool; }
    private:
        Tool m_tool;
    };
}

#endif // LEVEL_EDITOR_UI_EVENTS_H
