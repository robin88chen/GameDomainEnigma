/********************************************************************
 * \file   IGraphicAPI.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_INTERFACE_H
#define GRAPHIC_API_INTERFACE_H

#include "DeviceRequiredBits.h"
#include "TargetViewPort.h"
#include "GraphicAssetRepository.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "MathLib/AlgebraBasicTypes.h"
#include <system_error>

namespace Enigma::MathLib
{
    class ColorRGBA;
}

namespace Enigma::Graphics
{
    using error = std::error_code;

    class GraphicThread;
    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    using IBackSurfaceWeak = std::weak_ptr<IBackSurface>;
    class IDepthStencilSurface;
    using IDepthStencilSurfacePtr = std::shared_ptr<IDepthStencilSurface>;
    using IDepthStencilSurfaceWeak = std::weak_ptr<IDepthStencilSurface>;
    class IVertexShader;
    using IVertexShaderPtr = std::shared_ptr<IVertexShader>;
    class IPixelShader;
    using IPixelShaderPtr = std::shared_ptr<IPixelShader>;
    class TargetViewPort;
    class AssetRepository;

    class IGraphicAPI
    {
    public:
        enum class APIVersion
        {
            API_Dx9,
            API_Dx11,
            API_EGL,
        };
        enum class AsyncType : bool
        {
            UseAsyncDevice = true,
            NotAsyncDevice = false
        };
    public:
        IGraphicAPI();
        IGraphicAPI(const IGraphicAPI&) = delete;
        IGraphicAPI(IGraphicAPI&&) = delete;
        virtual ~IGraphicAPI();
        IGraphicAPI& operator=(const IGraphicAPI&) = delete;
        IGraphicAPI& operator=(IGraphicAPI&&) = delete;

        static IGraphicAPI* Instance();

        /** @name create / cleanup device */
        //@{
        virtual error CreateDevice(const DeviceRequiredBits& rqb, void* hwnd) = 0;
        virtual error CleanupDevice() = 0;
        virtual future_error AsyncCreateDevice(const DeviceRequiredBits& rqb, void* hwnd);
        virtual future_error AsyncCleanupDevice();
        //@}

        virtual error Flip() = 0;
        virtual future_error AsyncFlip();

        /** @name back / depth surface */
        //@{
        virtual error CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name) = 0;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt) = 0;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts) = 0;
        virtual error CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt) = 0;
        virtual error ShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth) = 0;
        virtual error ClearSurface(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) = 0;
        virtual future_error AsyncCreatePrimaryBackSurface(
            const std::string& back_name, const std::string& depth_name);
        virtual future_error AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt);
        virtual future_error AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts);
        virtual future_error AsyncCreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
            const GraphicFormat& fmt);
        virtual future_error AsyncShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth);
        virtual future_error AsyncClearSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value);
        //@}

        virtual error BindBackSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface) = 0;
        virtual future_error AsyncBindBackSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface);
        virtual IBackSurfacePtr CurrentBoundBackSurface() { return m_boundBackSurface.lock(); }
        virtual IDepthStencilSurfacePtr CurrentBoundDepthSurface() { return m_boundDepthSurface.lock(); }
        virtual error BindViewPort(const TargetViewPort& vp) = 0;
        virtual future_error AsyncBindViewPort(const TargetViewPort& vp);

        /** @name surface format */
        //@{
        const GraphicFormat& GetPrimaryBackSurfaceFormat() const { return m_fmtBackSurface; };
        const GraphicFormat& GetDepthSurfaceFormat() const { return m_fmtDepthSurface; };
        //@}

        /** @name Shader */
        //@{ 
        /** create vertex shader
        @param name shader name
        @param shader out shader interface
        */
        virtual error CreateVertexShader(const std::string& name) = 0;
        virtual future_error AsyncCreateVertexShader(const std::string& name);
        /** create pixel shader
        @param name shader name
        @param shader out shader interface
        */
        virtual error CreatePixelShader(const std::string& name) = 0;
        virtual future_error AsyncCreatePixelShader(const std::string& name);

        /** create shader program
        @param vtx_shader vertex shader
        @param pix_shader pixel shader
        @param program out shader program interface
        */
        virtual error CreateShaderProgram(const std::string& name,
            const IVertexShaderPtr& vtx_shader, const IPixelShaderPtr& pix_shader) = 0;
        virtual future_error AsyncCreateShaderProgram(const std::string& name,
            const IVertexShaderPtr& vtx_shader, const IPixelShaderPtr& pix_shader);

        /** create vertex declaration with data vertex format & effect */
        virtual error CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
            const IVertexShaderPtr& shader) = 0;
        virtual future_error AsyncCreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
            const IVertexShaderPtr& shader);
        /** query vertex declaration by data vertex format & effect */
        virtual std::string QueryVertexDeclarationName(const std::string& data_vertex_format,
            const IVertexShaderPtr& shader);

        /** @name Vertex/Index buffer */
        //@{
        /** create vertex buffer */
        virtual error CreateVertexBuffer(const std::string& buff_name) = 0;
        virtual future_error AsyncCreateVertexBuffer(const std::string& buff_name);
        /** create index buffer */
        virtual error CreateIndexBuffer(const std::string& buff_name) = 0;
        virtual future_error AsyncCreateIndexBuffer(const std::string& buff_name);
        //@}

        /** @name Device States */
        //@{
        /** create sampler state */
        virtual error CreateSamplerState(const std::string& name) = 0;
        virtual future_error AsyncCreateSamplerState(const std::string& name);
        /** create rasterizer state */
        virtual error CreateRasterizerState(const std::string& name) = 0;
        virtual future_error AsyncCreateRasterizerState(const std::string& name);
        /** create alpha blend state */
        virtual error CreateAlphaBlendState(const std::string& name) = 0;
        virtual future_error AsyncCreateAlphaBlendState(const std::string& name);
        /** create depth stencil state */
        virtual error CreateDepthStencilState(const std::string& name) = 0;
        virtual future_error AsyncCreateDepthStencilState(const std::string& name);

        //@}

        /** @name Textures */
        //@{
        /** create texture */
        virtual error CreateTexture(const std::string& tex_name) = 0;
        virtual future_error AsyncCreateTexture(const std::string& tex_name);
        /** create multi-texture */
        virtual error CreateMultiTexture(const std::string& tex_name) = 0;
        virtual future_error AsyncCreateMultiTexture(const std::string& tex_name);
        //@}

        virtual void TerminateGraphicThread();
        virtual GraphicThread* GetGraphicThread();

        /** Get graphic asset, assert if key not found */
        template <class T> T GetGraphicAsset(const std::string& asset_key)
        {
            return m_repository->Get<T>(asset_key);
        }

        /** Try get asset, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryGetGraphicAsset(const std::string& asset_key)
        {
            return m_repository->TryGetValue<T>(asset_key);
        }

        /** Remove graphic asset */
        void RemoveGraphicAsset(const std::string& asset_key) { m_repository->Remove(asset_key); }

    protected:
        /** @name surface format */
        //@{
        void SetPrimaryBackSurfaceFormat(const GraphicFormat& fmt) { m_fmtBackSurface = fmt; };
        void SetDepthSurfaceFormat(const GraphicFormat& fmt) { m_fmtDepthSurface = fmt; };
        //@}

    protected:
        static IGraphicAPI* m_instance;
        void* m_wnd;
        DeviceRequiredBits m_deviceRequiredBits;
        APIVersion m_apiVersion;

        GraphicFormat m_fmtBackSurface;
        GraphicFormat m_fmtDepthSurface;

        GraphicThread* m_workerThread;
        AssetRepository* m_repository;

        using VertexDeclMap = std::unordered_map<std::pair<std::string, std::string>, std::string, pair_hash>;
        VertexDeclMap m_vertexDeclMap;
        std::mutex m_declMapLock;

        IBackSurfaceWeak m_boundBackSurface;
        IDepthStencilSurfaceWeak m_boundDepthSurface;
        TargetViewPort m_boundViewPort;
    };
}

#endif // GRAPHIC_API_INTERFACE_H
