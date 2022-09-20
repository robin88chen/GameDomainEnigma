/*********************************************************************
 * \file   GraphicAPIDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_DX11_H
#define GRAPHIC_API_DX11_H

#include <D3D11.h>
#include "GraphicKernel/IGraphicAPI.h"

namespace Enigma::Graphics
{
    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    class IDepthStencilSurface;
    using IDepthStencilSurfacePtr = std::shared_ptr<IDepthStencilSurface>;
}
namespace Enigma::Devices
{
    class DeviceCreatorDx11;
    class SwapChainDx11;
    class AdapterDx11;

    using error = std::error_code;

    class GraphicAPIDx11 : public Graphics::IGraphicAPI
    {
    public:
        GraphicAPIDx11();
        GraphicAPIDx11(const GraphicAPIDx11&) = delete;
        GraphicAPIDx11(GraphicAPIDx11&&) = delete;
        virtual ~GraphicAPIDx11() override;
        GraphicAPIDx11& operator=(const GraphicAPIDx11&) = delete;
        GraphicAPIDx11& operator=(GraphicAPIDx11&&) = delete;

        virtual void CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd) override;
        virtual void CleanupDevice() override;

        virtual void BeginScene() override;
        virtual void EndScene() override;

        virtual void CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name) override;

        virtual void CreateVertexShader(const std::string& name) override;
        virtual void CreatePixelShader(const std::string& name) override;
        virtual void CreateShaderProgram(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader,
            const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl) override;
        virtual void CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
            const Graphics::IVertexShaderPtr& shader) override;

        virtual void CreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex, unsigned int sizeBuffer) override;
        virtual void CreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer) override;

        virtual void Clear(const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) override;
        virtual void Flip() override;

        ID3D11Texture2D* GetPrimaryD3DBackbuffer();

        ID3D11Device* GetD3DDevice() { return m_d3dDevice; };
        ID3D11DeviceContext* GetD3DDeviceContext() { return m_d3dDeviceContext; }
        SwapChainDx11* GetSwapChain() { return m_swapChain; }

    private:
        virtual error DrawPrimitive(unsigned int vertexCount, unsigned int vertexOffset) override;
        virtual error DrawIndexedPrimitive(
            unsigned int indexCount, unsigned int vertexCount, unsigned int indexOffset,
            int baseVertexOffset) override;
        //virtual error FlipBackSurface() override;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt) override;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, unsigned int buff_count,
            const std::vector<Graphics::GraphicFormat>& fmts) override;
        virtual error CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt) override;
        virtual error ShareDepthStencilSurface(const std::string& depth_name,
            const Graphics::IDepthStencilSurfacePtr& from_depth) override;
        //virtual error ClearSurface(const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface,
          //  const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) override;
        virtual error CreateSamplerState(const std::string& name, const Graphics::IDeviceSamplerState::SamplerStateData& data) override;
        virtual error CreateRasterizerState(const std::string& name, const Graphics::IDeviceRasterizerState::RasterizerStateData& data) override;
        virtual error CreateAlphaBlendState(const std::string& name, const Graphics::IDeviceAlphaBlendState::BlendStateData& data) override;
        virtual error CreateDepthStencilState(const std::string& name, const Graphics::IDeviceDepthStencilState::DepthStencilData& data) override;
        virtual error CreateTexture(const std::string& tex_name) override;
        virtual error CreateMultiTexture(const std::string& tex_name) override;

        virtual error BindBackSurface(
            const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface) override;
        virtual error BindViewPort(const Graphics::TargetViewPort& vp) override;
        virtual error BindVertexDeclaration(const Graphics::IVertexDeclarationPtr& vertexDecl) override;
        virtual error BindVertexShader(const Graphics::IVertexShaderPtr& shader) override;
        virtual error BindPixelShader(const Graphics::IPixelShaderPtr& shader) override;
        virtual error BindShaderProgram(const Graphics::IShaderProgramPtr& shader) override;
        virtual error BindVertexBuffer(const Graphics::IVertexBufferPtr& buffer, Graphics::PrimitiveTopology pt) override;
        virtual error BindIndexBuffer(const Graphics::IIndexBufferPtr& buffer) override;

        void CleanupDeviceObjects();

        error ClearSingleBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color);
        error ClearMultiBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color);
        error ClearDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& depth_surface,
            float depth_value, unsigned int stencil_value);

        error BindSingleBackSurface(
            const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface);
        error BindMultiBackSurface(
            const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface);

        void AddDebugInfoFilter();

    private:
        DeviceCreatorDx11* m_creator;
        SwapChainDx11* m_swapChain;
        AdapterDx11* m_adapter;
        // D3D Device
        ID3D11Device* m_d3dDevice;
        ID3D11DeviceContext* m_d3dDeviceContext;
    };
}

#endif // GRAPHIC_API_DX11_H
