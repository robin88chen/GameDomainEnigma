/*********************************************************************
 * \file   GraphicCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef GRAPHIC_COMMANDS_H
#define GRAPHIC_COMMANDS_H

#include "DeviceRequiredBits.h"
#include "Frameworks/Command.h"

namespace Enigma::Graphics
{
    class CreateDevice : public Frameworks::ICommand
    {
    public:
        CreateDevice(const DeviceRequiredBits& rqb, void* hwnd) : m_rqb(rqb), m_hwnd(hwnd) {}
        const DeviceRequiredBits& GetRequiredBits() const  { return m_rqb; }
        void* GetHwnd() const { return m_hwnd; }

    private:
        DeviceRequiredBits m_rqb;
        void* m_hwnd;
    };
    class CleanupDevice : public Frameworks::ICommand
    {
    };
    class BeginScene : public Frameworks::ICommand
    {
    };
    class EndScene : public Frameworks::ICommand
    {
    };
    class DrawPrimitive : public Frameworks::ICommand
    {
    public:
        DrawPrimitive(unsigned int vertexCount, unsigned int vertexOffset) : m_count(vertexCount), m_offset(vertexOffset) {}
        unsigned int GetVertexCount() const { return m_count; }
        unsigned int GetVertexOffset() const { return m_offset; }

    private:
        unsigned int m_count;
        unsigned int m_offset;
    };
    class DrawIndexedPrimitive : public Frameworks::ICommand
    {
    public:
        DrawIndexedPrimitive(unsigned int indexCount, unsigned int vertexCount, unsigned int indexOffset, int baseVertexOffset)
            : m_indexCount(indexCount), m_vertexCount(vertexCount),m_indexOffset(indexOffset), m_baseVertexOffset(baseVertexOffset) {}
        unsigned int GetIndexCount() const { return m_indexCount; }
        unsigned int GetVertexCount() const { return m_vertexCount; }
        unsigned int GetIndexOffset() const { return m_indexOffset; }
        int GetBaseVertexOffset() const { return m_baseVertexOffset; }

    private:
        unsigned int m_indexCount;
        unsigned int m_vertexCount;
        unsigned int m_indexOffset;
        int m_baseVertexOffset;
    };
}


#endif // GRAPHIC_COMMANDS_H
