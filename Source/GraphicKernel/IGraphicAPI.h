﻿/********************************************************************
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
#include "GraphicAssetStash.h"
#include "IDeviceSamplerState.h"
#include "IDeviceAlphaBlendState.h"
#include "IDeviceRasterizerState.h"
#include "IDeviceDepthStencilState.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "Frameworks/CommandSubscriber.h"
#include "MathLib/AlgebraBasicTypes.h"
#include <system_error>
#include <mutex>
#include <memory>


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
    class IVertexDeclaration;
    using IVertexDeclarationPtr = std::shared_ptr<IVertexDeclaration>;
    class IShaderProgram;
    using IShaderProgramPtr = std::shared_ptr<IShaderProgram>;
    class IVertexBuffer;
    using IVertexBufferPtr = std::shared_ptr<IVertexBuffer>;
    class IIndexBuffer;
    using IIndexBufferPtr = std::shared_ptr<IIndexBuffer>;
    class TargetViewPort;
    class AssetRepository;

    class IGraphicAPI
    {
    public:
        enum class APIVersion
        {
            API_Unknown,
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
        IGraphicAPI(AsyncType async);
        IGraphicAPI(const IGraphicAPI&) = delete;
        IGraphicAPI(IGraphicAPI&&) = delete;
        virtual ~IGraphicAPI();
        IGraphicAPI& operator=(const IGraphicAPI&) = delete;
        IGraphicAPI& operator=(IGraphicAPI&&) = delete;

        static IGraphicAPI* instance();

        APIVersion GetAPIVersion() { return m_apiVersion; }

        virtual void beginScene();
        virtual void endScene();
        virtual void draw(unsigned int vertexCount, unsigned int vertexOffset);
        virtual void draw(unsigned int indexCount, unsigned int vertexCount, unsigned int indexOffset,
            int baseVertexOffset);
        virtual void clear(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value);
        virtual void flip();

        virtual void bind(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface);
        virtual void bind(const TargetViewPort& vp);
        virtual void bind(const IShaderProgramPtr& shader);
        virtual void bind(const IVertexBufferPtr& buffer, PrimitiveTopology pt);
        virtual void bind(const IIndexBufferPtr& buffer);

        bool UseAsync() const { return m_async == AsyncType::UseAsyncDevice; }
        virtual const DeviceRequiredBits& GetDeviceRequiredBits() { return m_deviceRequiredBits; };

        virtual const IBackSurfacePtr& CurrentBoundBackSurface() const { return m_boundBackSurface; }
        virtual const IDepthStencilSurfacePtr& CurrentBoundDepthSurface() const { return m_boundDepthSurface; }

        /** @name surface format */
        //@{
        const GraphicFormat& GetPrimaryBackSurfaceFormat() const { return m_fmtBackSurface; };
        const GraphicFormat& getDepthSurfaceFormat() const { return m_fmtDepthSurface; };
        //@}
        virtual const IShaderProgramPtr& CurrentBoundShaderProgram() const { return m_boundShaderProgram; };
        virtual const IVertexBufferPtr& CurrentBoundVertexBuffer() const { return m_boundVertexBuffer; };
        virtual PrimitiveTopology CurrentBoundTopology() { return m_boundTopology; }
        virtual const IIndexBufferPtr& CurrentBoundIndexBuffer() const { return m_boundIndexBuffer; };

        virtual void TerminateGraphicThread();
        virtual GraphicThread* GetGraphicThread();
        virtual bool IsValidGraphicThread(const std::thread::id& id);

        /** Get graphic asset, assert if key not found */
        template <class T> T GetGraphicAsset(const std::string& asset_key)
        {
            return m_stash->get<T>(asset_key);
        }

        /** Try find graphic asset, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryFindGraphicAsset(const std::string& key)
        {
            return m_stash->TryFindValue<T>(key);
        }

        bool HasGraphicAsset(const std::string& asset_key)
        {
            return m_stash->HasData(asset_key);
        }

    protected:
        void subscribeHandlers();
        void unsubscribeHandlers();

        /** command handlers */
        //@{
        void createDevice(const Frameworks::ICommandPtr& c);
        void cleanDevice(const Frameworks::ICommandPtr& c);

        void beginScene(const Frameworks::ICommandPtr& c);
        void endScene(const Frameworks::ICommandPtr& c);
        void drawPrimitive(const Frameworks::ICommandPtr& c);
        void drawIndexedPrimitive(const Frameworks::ICommandPtr& c);
        void clear(const Frameworks::ICommandPtr& c);
        void flip(const Frameworks::ICommandPtr& c);

        void createPrimarySurface(const Frameworks::ICommandPtr& c);
        void createBackSurface(const Frameworks::ICommandPtr& c);
        void DoCreatingMultiBackSurface(const Frameworks::ICommandPtr& c);
        void DoCreatingDepthSurface(const Frameworks::ICommandPtr& c);
        void DoSharingDepthSurface(const Frameworks::ICommandPtr& c);

        void DoCreatingVertexShader(const Frameworks::ICommandPtr& c);
        void DoCreatingPixelShader(const Frameworks::ICommandPtr& c);
        void DoCreatingShaderProgram(const Frameworks::ICommandPtr& c);
        void DoCreatingVertexDeclaration(const Frameworks::ICommandPtr& c);

        void DoCreatingVertexBuffer(const Frameworks::ICommandPtr& c);
        void DoCreatingIndexBuffer(const Frameworks::ICommandPtr& c);

        void DoCreatingSamplerState(const Frameworks::ICommandPtr& c);
        void DoCreatingRasterizerState(const Frameworks::ICommandPtr& c);
        void DoCreatingBlendState(const Frameworks::ICommandPtr& c);
        void DoCreatingDepthStencilState(const Frameworks::ICommandPtr& c);

        void createTexture(const Frameworks::ICommandPtr& c);
        void createMultiTexture(const Frameworks::ICommandPtr& c);

        void DoBindingBackSurface(const Frameworks::ICommandPtr& c);
        void DoBindingViewPort(const Frameworks::ICommandPtr& c);

        void DoBindingShaderProgram(const Frameworks::ICommandPtr& c);

        void DoBindingVertexBuffer(const Frameworks::ICommandPtr& c);
        void DoBindingIndexBuffer(const Frameworks::ICommandPtr& c);
        //@}

        /** @name create / cleanup device */
        //@{
        virtual error createDevice(const DeviceRequiredBits& rqb, void* hwnd) = 0;
        virtual error cleanupDevice() = 0;
        virtual future_error asyncCreateDevice(const DeviceRequiredBits& rqb, void* hwnd);
        virtual future_error asyncCleanupDevice();

        /** @name scene begin/end  */
        //@{
        virtual error beginDrawingScene() = 0;
        virtual error endDrawingScene() = 0;
        virtual future_error asyncBeginDrawingScene();
        virtual future_error asyncEndDrawingScene();
        //@}

        /** @name draw call */
        //@{
        /** draw primitive */
        virtual error drawPrimitive(unsigned int vertexCount, unsigned int vertexOffset) = 0;
        virtual future_error asyncDrawPrimitive(unsigned int vertexCount, unsigned int vertexOffset);
        /** draw indexed primitive */
        virtual error drawIndexedPrimitive(unsigned int indexCount, unsigned int vertexCount, unsigned int indexOffset,
            int baseVertexOffset) = 0;
        virtual future_error asyncDrawIndexedPrimitive(unsigned int indexCount, unsigned int vertexCount, unsigned int indexOffset,
            int baseVertexOffset);
        //@}

        virtual error flipBackSurface() = 0;
        virtual future_error asyncFlipBackSurface();

        /** @name back / depth surface */
        //@{
        virtual error CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name) = 0;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
            const GraphicFormat& fmt) = 0;
        virtual error CreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts) = 0;
        virtual error CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension<unsigned>& dimension,
            const GraphicFormat& fmt) = 0;
        virtual error ShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth) = 0;
        virtual error ClearSurface(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value) = 0;
        virtual future_error AsyncCreatePrimaryBackSurface(
            const std::string& back_name, const std::string& depth_name);
        virtual future_error AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
            const GraphicFormat& fmt);
        virtual future_error AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
            unsigned int buff_count, const std::vector<GraphicFormat>& fmts);
        virtual future_error AsyncCreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension<unsigned>& dimension,
            const GraphicFormat& fmt);
        virtual future_error AsyncShareDepthStencilSurface(const std::string& depth_name, const IDepthStencilSurfacePtr& from_depth);
        virtual future_error AsyncClearSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
            const MathLib::ColorRGBA& color, float depth_value, unsigned int stencil_value);
        //@}

        /** @name Shader */
        //@{
        /** create vertex shader
        @param name shader name
        */
        virtual error CreateVertexShader(const std::string& name) = 0;
        virtual future_error AsyncCreateVertexShader(const std::string& name);
        /** create pixel shader
        @param name shader name
        */
        virtual error CreatePixelShader(const std::string& name) = 0;
        virtual future_error AsyncCreatePixelShader(const std::string& name);

        /** create shader program
        @param vtx_shader vertex shader
        @param pix_shader pixel shader
        */
        virtual error CreateShaderProgram(const std::string& name,
            const IVertexShaderPtr& vtx_shader, const IPixelShaderPtr& pix_shader, const IVertexDeclarationPtr& vtx_decl) = 0;
        virtual future_error AsyncCreateShaderProgram(const std::string& name,
            const IVertexShaderPtr& vtx_shader, const IPixelShaderPtr& pix_shader, const IVertexDeclarationPtr& vtx_decl);

        /** create vertex declaration with data vertex format & effect */
        virtual error CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
            const IVertexShaderPtr& shader) = 0;
        virtual future_error AsyncCreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
            const IVertexShaderPtr& shader);

        /** @name Vertex/Index buffer */
        //@{
        /** create vertex buffer */
        virtual error CreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex, unsigned int sizeBuffer) = 0;
        virtual future_error AsyncCreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex, unsigned int sizeBuffer);
        /** create index buffer */
        virtual error CreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer) = 0;
        virtual future_error AsyncCreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer);
        //@}

        /** @name Device States */
        //@{
        /** create sampler state */
        virtual error CreateSamplerState(const std::string& name, const IDeviceSamplerState::SamplerStateData& data) = 0;
        virtual future_error AsyncCreateSamplerState(const std::string& name, const IDeviceSamplerState::SamplerStateData& data);
        /** create rasterizer state */
        virtual error CreateRasterizerState(const std::string& name, const IDeviceRasterizerState::RasterizerStateData& data) = 0;
        virtual future_error AsyncCreateRasterizerState(const std::string& name, const IDeviceRasterizerState::RasterizerStateData& data);
        /** create alpha blend state */
        virtual error CreateAlphaBlendState(const std::string& name, const IDeviceAlphaBlendState::BlendStateData& data) = 0;
        virtual future_error AsyncCreateAlphaBlendState(const std::string& name, const IDeviceAlphaBlendState::BlendStateData& data);
        /** create depth stencil state */
        virtual error CreateDepthStencilState(const std::string& name, const IDeviceDepthStencilState::DepthStencilData& data) = 0;
        virtual future_error AsyncCreateDepthStencilState(const std::string& name, const IDeviceDepthStencilState::DepthStencilData& data);
        //@}

        /** @name Textures */
        //@{
        /** create texture */
        virtual error createTexture(const std::string& tex_name) = 0;
        virtual future_error asyncCreateTexture(const std::string& tex_name);
        /** create multi-texture */
        virtual error createMultiTexture(const std::string& tex_name) = 0;
        virtual future_error asyncCreateMultiTexture(const std::string& tex_name);
        //@}

        virtual error BindBackSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface) = 0;
        virtual future_error AsyncBindBackSurface(
            const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface);
        virtual error bindViewPort(const TargetViewPort& vp) = 0;
        virtual future_error AsyncBindViewPort(const TargetViewPort& vp);

        /** @name bind shader */
        //@{
        virtual error BindVertexShader(const IVertexShaderPtr& shader) = 0;
        virtual error BindPixelShader(const IPixelShaderPtr& shader) = 0;
        virtual error BindShaderProgram(const IShaderProgramPtr& shader) = 0;
        virtual error BindVertexDeclaration(const IVertexDeclarationPtr& vertexDecl) = 0;
        virtual future_error AsyncBindShaderProgram(const IShaderProgramPtr& shader);
        //@}

        /** @name bind vertex / index buffer */
        //@{
        virtual error BindVertexBuffer(const IVertexBufferPtr& buffer, PrimitiveTopology pt) = 0;
        virtual future_error AsyncBindVertexBuffer(const IVertexBufferPtr& buffer, PrimitiveTopology pt);
        virtual error BindIndexBuffer(const IIndexBufferPtr& buffer) = 0;
        virtual future_error AsyncBindIndexBuffer(const IIndexBufferPtr& buffer);
        //@}

        //@}
        /** @name surface format */
        //@{
        void SetPrimaryBackSurfaceFormat(const GraphicFormat& fmt) { m_fmtBackSurface = fmt; };
        void SetDepthSurfaceFormat(const GraphicFormat& fmt) { m_fmtDepthSurface = fmt; };
        //@}

    protected:
        static IGraphicAPI* m_instance;
        AsyncType m_async;
        void* m_wnd;
        DeviceRequiredBits m_deviceRequiredBits;
        APIVersion m_apiVersion;

        GraphicFormat m_fmtBackSurface;
        GraphicFormat m_fmtDepthSurface;

        GraphicThread* m_workerThread;
        AssetStash* m_stash;

        IBackSurfacePtr m_boundBackSurface;
        IDepthStencilSurfacePtr m_boundDepthSurface;
        IVertexDeclarationPtr m_boundVertexDecl;
        IVertexShaderPtr m_boundVertexShader;
        IPixelShaderPtr m_boundPixelShader;
        IShaderProgramPtr m_boundShaderProgram;
        IVertexBufferPtr m_boundVertexBuffer;
        PrimitiveTopology m_boundTopology;
        IIndexBufferPtr m_boundIndexBuffer;
        TargetViewPort m_boundViewPort;

        Frameworks::CommandSubscriberPtr m_createDevice;
        Frameworks::CommandSubscriberPtr m_cleanDevice;

        Frameworks::CommandSubscriberPtr m_beginScene;
        Frameworks::CommandSubscriberPtr m_endScene;
        Frameworks::CommandSubscriberPtr m_drawPrimitive;
        Frameworks::CommandSubscriberPtr m_drawIndexedPrimitive;
        Frameworks::CommandSubscriberPtr m_clear;
        Frameworks::CommandSubscriberPtr m_flip;

        Frameworks::CommandSubscriberPtr m_createPrimarySurface;
        Frameworks::CommandSubscriberPtr m_createBackSurface;
        Frameworks::CommandSubscriberPtr m_doCreatingMultiBackSurface;
        Frameworks::CommandSubscriberPtr m_doCreatingDepthSurface;
        Frameworks::CommandSubscriberPtr m_doSharingDepthSurface;

        Frameworks::CommandSubscriberPtr m_doCreatingVertexShader;
        Frameworks::CommandSubscriberPtr m_doCreatingPixelShader;
        Frameworks::CommandSubscriberPtr m_doCreatingShaderProgram;
        Frameworks::CommandSubscriberPtr m_doCreatingVertexDeclaration;

        Frameworks::CommandSubscriberPtr m_doCreatingVertexBuffer;
        Frameworks::CommandSubscriberPtr m_doCreatingIndexBuffer;

        Frameworks::CommandSubscriberPtr m_doCreatingSamplerState;
        Frameworks::CommandSubscriberPtr m_doCreatingRasterizerState;
        Frameworks::CommandSubscriberPtr m_doCreatingDepthStencilState;
        Frameworks::CommandSubscriberPtr m_doCreatingBlendState;

        Frameworks::CommandSubscriberPtr m_createTexture;
        Frameworks::CommandSubscriberPtr m_createMultiTexture;

        Frameworks::CommandSubscriberPtr m_doBindingBackSurface;
        Frameworks::CommandSubscriberPtr m_doBindingViewPort;

        Frameworks::CommandSubscriberPtr m_doBindingShaderProgram;

        Frameworks::CommandSubscriberPtr m_doBindingVertexBuffer;
        Frameworks::CommandSubscriberPtr m_doBindingIndexBuffer;
    };
}

#endif // GRAPHIC_API_INTERFACE_H
