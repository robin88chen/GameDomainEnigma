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
#include "TargetViewPort.h"
#include "IVertexShader.h"
#include "IPixelShader.h"
#include "IShaderProgram.h"
#include "Frameworks/Command.h"
#include "MathLib/ColorRGBA.h"

namespace Enigma::Graphics
{
    /** device create / cleanup */
    class CreateDevice : public Frameworks::ICommand
    {
    public:
        CreateDevice(const DeviceRequiredBits& rqb, void* hwnd) : m_rqb(rqb), m_hwnd(hwnd) {}
        const DeviceRequiredBits& GetRequiredBits() const { return m_rqb; }
        void* GetHwnd() const { return m_hwnd; }

    private:
        DeviceRequiredBits m_rqb;
        void* m_hwnd;
    };
    class CleanupDevice : public Frameworks::ICommand
    {
    };

    /** scene begin / end */
    class BeginScene : public Frameworks::ICommand
    {
    };
    class EndScene : public Frameworks::ICommand
    {
    };

    /** draw (indexed) primitive */
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
            : m_indexCount(indexCount), m_vertexCount(vertexCount), m_indexOffset(indexOffset), m_baseVertexOffset(baseVertexOffset) {}
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

    /** clear */
    class ClearSurface : public Frameworks::ICommand
    {
    public:
        ClearSurface(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value)
            : m_backSurface(back_surface), m_depthSurface(depth_surface), m_color(color), m_depthValue(depth_value),
            m_stencilValue(stencil_value) {};
        const IBackSurfacePtr& GetBackSurface() const { return m_backSurface; }
        const IDepthStencilSurfacePtr& GetDepthSurface() const { return m_depthSurface; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }
        float GetDepthValue() const { return m_depthValue; }
        unsigned int GetStencilValue() const { return m_stencilValue; }
    private:
        IBackSurfacePtr m_backSurface;
        IDepthStencilSurfacePtr m_depthSurface;
        MathLib::ColorRGBA m_color;
        float m_depthValue;
        unsigned int m_stencilValue;
    };
    /** flip */
    class FlipBackSurface : public Frameworks::ICommand
    {
    };

    /** create surface */
    class CreatePrimarySurface : public Frameworks::ICommand
    {
    public:
        CreatePrimarySurface(const std::string& back_name, const std::string& depth_name)
    		: m_backName(back_name), m_depthName(depth_name) {};
        const std::string& GetBacksurfaceName() const { return m_backName; }
        const std::string& GetDepthsurfaceName() const { return m_depthName; }
    private:
        std::string m_backName;
        std::string m_depthName;
    };
    class CreateBacksurface : public Frameworks::ICommand
    {
    public:
        CreateBacksurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt) : m_backName(back_name), m_dimension(dimension), m_fmt(fmt) {};
        const std::string& GetBacksurfaceName() const { return m_backName; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }
        const GraphicFormat& GetFormat() const { return m_fmt; }
    private:
        std::string m_backName;
        MathLib::Dimension m_dimension;
        GraphicFormat m_fmt;
    };
    class CreateMultiBacksurface : public Frameworks::ICommand
    {
    public:
        CreateMultiBacksurface(const std::string& back_name, const MathLib::Dimension& dimension,
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts)
    			: m_backName(back_name), m_dimension(dimension), m_buffCount(buff_count), m_fmts(fmts) {};
        const std::string& GetBacksurfaceName() const { return m_backName; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }
        unsigned int GetSurfaceCount() const { return m_buffCount; }
        const std::vector<GraphicFormat>& GetFormats() const { return m_fmts; }
    private:
        std::string m_backName;
        MathLib::Dimension m_dimension;
        unsigned int m_buffCount;
        std::vector<GraphicFormat> m_fmts;
    };
    class CreateDepthStencilSurface : public Frameworks::ICommand
    {
    public:
        CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt) : m_depthName(depth_name), m_dimension(dimension), m_fmt(fmt) {};
        const std::string& GetDepthStencilSurfaceName() const { return m_depthName; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }
        const GraphicFormat& GetFormat() const { return m_fmt; }
    private:
        std::string m_depthName;
        MathLib::Dimension m_dimension;
        GraphicFormat m_fmt;
    };
    class ShareDepthStencilSurface : public Frameworks::ICommand
    {
    public:
        ShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth)
    	: m_depthName(depth_name), m_fromDepth(from_depth) {};
        const std::string& GetDepthStencilSurfaceName() const { return m_depthName; }
        const IDepthStencilSurfacePtr& GetSourceDepth() const { return m_fromDepth; }
    private:
        std::string m_depthName;
        IDepthStencilSurfacePtr m_fromDepth;
    };

    /** resize surface */
    class ResizeBackSurface : public Frameworks::ICommand
    {
    public:
        ResizeBackSurface(const std::string& name, const MathLib::Dimension& dimension) : m_name(name), m_dimension(dimension) {};
        const std::string& GetName() const { return m_name; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }
    private:
        std::string m_name;
        MathLib::Dimension m_dimension;
    };
    class ResizeDepthSurface : public Frameworks::ICommand
    {
    public:
        ResizeDepthSurface(const std::string& name, const MathLib::Dimension& dimension) : m_name(name), m_dimension(dimension) {};
        const std::string& GetName() const { return m_name; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }
    private:
        std::string m_name;
        MathLib::Dimension m_dimension;
    };

    /** create shaders */
    class CreateVertexShader : public Frameworks::ICommand
    {
    public:
        CreateVertexShader(const std::string& name) : m_name(name) {};
        const std::string& GetName() const { return m_name; }
    private:
        std::string m_name;
    };
    class CreatePixelShader : public Frameworks::ICommand
    {
    public:
        CreatePixelShader(const std::string& name) : m_name(name) {};
        const std::string& GetName() const { return m_name; }
    private:
        std::string m_name;
    };
    class CreateShaderProgram : public Frameworks::ICommand
    {
    public:
        CreateShaderProgram(const std::string& name, const IVertexShaderPtr& vtx_shader, 
            const IPixelShaderPtr& pixel_shader, const IVertexDeclarationPtr& vtx_decl)
            : m_name(name), m_vtxShader(vtx_shader), m_pixelShader(pixel_shader), m_vtxDecl(vtx_decl) {};
        const std::string& GetName() const { return m_name; }
        const IVertexShaderPtr& GetVertexShader() const { return m_vtxShader; }
        const IPixelShaderPtr& GetPixelShader() const { return m_pixelShader; }
        const IVertexDeclarationPtr& GetVertexDeclaration() const { return m_vtxDecl; }
    private:
        std::string m_name;
        IVertexShaderPtr m_vtxShader;
        IPixelShaderPtr m_pixelShader;
        IVertexDeclarationPtr m_vtxDecl;
    };

    /** bind surface / viewport */
    class BindBackSurface : public Frameworks::ICommand
    {
    public:
        BindBackSurface(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface)
            : m_backSurface(back_surface), m_depthSurface(depth_surface) {};
        const IBackSurfacePtr& GetBackSurface() const { return m_backSurface; }
        const IDepthStencilSurfacePtr& GetDepthSurface() const { return m_depthSurface; }
    private:
        IBackSurfacePtr m_backSurface;
        IDepthStencilSurfacePtr m_depthSurface;
    };
    class BindViewPort : public Frameworks::ICommand
    {
    public:
        BindViewPort(const TargetViewPort& vp) : m_vp(vp) {};
        const TargetViewPort& GetViewPort() const { return m_vp; }
    private:
        TargetViewPort m_vp;
    };
}


#endif // GRAPHIC_COMMANDS_H
