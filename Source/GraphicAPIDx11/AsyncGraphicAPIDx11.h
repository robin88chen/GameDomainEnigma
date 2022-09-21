/*********************************************************************
 * \file   AsyncGraphicAPIDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef ASYNC_GRAPHIC_API_DX11_H
#define ASYNC_GRAPHIC_API_DX11_H

#include "GraphicKernel/IGraphicAPI.h"
#include <D3D11.h>
#include <system_error>

#include "GraphicAPIDx11.h"

namespace Enigma::Devices
{
    using error = std::error_code;

    class GraphicAPIDx11;
    class SwapChainDx11;

    class AsyncGraphicAPIDx11 : public GraphicAPIDx11
    {
    public:
        AsyncGraphicAPIDx11();
        AsyncGraphicAPIDx11(const AsyncGraphicAPIDx11&) = delete;
        AsyncGraphicAPIDx11(AsyncGraphicAPIDx11&&) = delete;
        virtual ~AsyncGraphicAPIDx11() override;
        AsyncGraphicAPIDx11& operator=(const AsyncGraphicAPIDx11&) = delete;
        AsyncGraphicAPIDx11& operator=(AsyncGraphicAPIDx11&&) = delete;

        virtual const std::unique_ptr<Graphics::IShaderCompiler>& GetShaderCompiler() override;

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

        ID3D11Device* GetD3DDevice();
        ID3D11DeviceContext* GetD3DDeviceContext();
        SwapChainDx11* GetSwapChain();

    private:
        virtual error DrawPrimitive(unsigned int vertexCount, unsigned int vertexOffset) override { return error{}; };
        virtual error DrawIndexedPrimitive(
            unsigned int indexCount, unsigned int vertexCount, unsigned int indexOffset,
            int baseVertexOffset) override { return error{}; };
        //virtual error FlipBackSurface() override { return error{}; };
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt) override { return error{}; };
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, unsigned int buff_count,
            const std::vector<Graphics::GraphicFormat>& fmts) override { return error{}; };
        virtual error CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const Graphics::GraphicFormat& fmt) override { return error{}; };
        virtual error ShareDepthStencilSurface(const std::string& depth_name,
            const Graphics::IDepthStencilSurfacePtr& from_depth) override { return error{}; };
        //virtual error ClearSurface(const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface,
            //const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) override { return error{}; };
        //virtual error CreateVertexShader(const std::string& name) override { return error{}; };
        //virtual error CreatePixelShader(const std::string& name) override { return error{}; };
        //virtual error CreateShaderProgram(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader,
          //  const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl) override { return error{}; };
        //virtual error CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
          //  const Graphics::IVertexShaderPtr& shader) override { return error{}; };
        virtual error CreateSamplerState(const std::string& name, const Graphics::IDeviceSamplerState::SamplerStateData& data) override { return error{}; };
        virtual error CreateRasterizerState(const std::string& name, const Graphics::IDeviceRasterizerState::RasterizerStateData& data) override { return error{}; };
        virtual error CreateAlphaBlendState(const std::string& name, const Graphics::IDeviceAlphaBlendState::BlendStateData& data) override { return error{}; };
        virtual error CreateDepthStencilState(const std::string& name, const Graphics::IDeviceDepthStencilState::DepthStencilData& data) override { return error{}; };
        virtual error CreateTexture(const std::string& tex_name) override { return error{}; };
        virtual error CreateMultiTexture(const std::string& tex_name) override { return error{}; };

        virtual error BindBackSurface(
            const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface) override { return error{}; };
        virtual error BindViewPort(const Graphics::TargetViewPort& vp) override { return error{}; };
        virtual error BindVertexDeclaration(const Graphics::IVertexDeclarationPtr& vertexDecl) override { return error{}; };
        virtual error BindVertexShader(const Graphics::IVertexShaderPtr& shader) override { return error{}; };
        virtual error BindPixelShader(const Graphics::IPixelShaderPtr& shader) override { return error{}; };
        virtual error BindShaderProgram(const Graphics::IShaderProgramPtr& shader) override { return error{}; };
        virtual error BindVertexBuffer(const Graphics::IVertexBufferPtr& buffer, Graphics::PrimitiveTopology pt) override { return error{}; };
        virtual error BindIndexBuffer(const Graphics::IIndexBufferPtr& buffer) override { return error{}; };

        void CleanupDeviceObjects() {};

        error ClearSingleBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color) { return error{}; };
        error ClearMultiBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color) { return error{}; };
        error ClearDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& depth_surface,
            float depth_value, unsigned int stencil_value) {
            return error{};
        };

        error BindSingleBackSurface(
            const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface) {
            return error{};
        };
        error BindMultiBackSurface(
            const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface) {
            return error{};
        };

        void AddDebugInfoFilter() {};
    };
}

#endif // ASYNC_GRAPHIC_API_DX11_H
