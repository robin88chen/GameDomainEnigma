/*********************************************************************
 * \file   GraphicEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_EVENTS_H
#define GRAPHIC_EVENTS_H

#include "Frameworks/Event.h"
#include "MathLib/Rect.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::Graphics
{
    class IDeviceSamplerState;
    class IDeviceRasterizerState;
    class IDeviceAlphaBlendState;
    class IDeviceDepthStencilState;
    class ITexture;

    //------------------------ Device --------------------//
    class DeviceCreated : public Frameworks::IEvent
    {
    };

    //----------------------- Surfaces -------------------//
    class PrimarySurfaceCreated : public Frameworks::IEvent
    {
    public:
        PrimarySurfaceCreated(const std::string& back_name, const std::string& depth_name) :
            m_backSurfaceName(back_name), m_depthSurfaceName(depth_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_backSurfaceName;
        std::string m_depthSurfaceName;
    };
    class BackSurfaceCreated : public Frameworks::IEvent
    {
    public:
        BackSurfaceCreated(const std::string& back_name) :
            m_backSurfaceName(back_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_backSurfaceName;
    };
    class BackSurfaceResized : public Frameworks::IEvent
    {
    public:
        BackSurfaceResized(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension) :
            m_backSurfaceName(back_name), m_dimension(dimension) {};
        const std::string& GetSurfaceName() const { return m_backSurfaceName; }
        const MathLib::Dimension<unsigned>& GetDimension() const { return m_dimension; }
    private:
        std::string m_backSurfaceName;
        MathLib::Dimension<unsigned> m_dimension;
    };
    class MultiBackSurfaceCreated : public Frameworks::IEvent
    {
    public:
        MultiBackSurfaceCreated(const std::string& back_name) :
            m_backSurfaceName(back_name) {};
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_backSurfaceName;
    };
    class DepthSurfaceCreated : public Frameworks::IEvent
    {
    public:
        DepthSurfaceCreated(const std::string& depth_name) :
            m_depthSurfaceName(depth_name) {};
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_depthSurfaceName;
    };
    class DepthSurfaceResized : public Frameworks::IEvent
    {
    public:
        DepthSurfaceResized(const std::string& depth_name, const MathLib::Dimension<unsigned>& dimension) :
            m_depthSurfaceName(depth_name), m_dimension(dimension) {};
        const std::string& GetSurfaceName() const { return m_depthSurfaceName; }
        const MathLib::Dimension<unsigned>& GetDimension() const { return m_dimension; }
    private:
        std::string m_depthSurfaceName;
        MathLib::Dimension<unsigned> m_dimension;
    };
    class DepthSurfaceShared : public Frameworks::IEvent
    {
    public:
        DepthSurfaceShared(const std::string& depth_name) :
            m_depthSurfaceName(depth_name) {};
        const std::string& GetDepthSurfaceName() { return m_depthSurfaceName; }
    private:
        std::string m_depthSurfaceName;
    };

    //---------------- Textures -----------------------//
    class DeviceTextureCreated : public Frameworks::IEvent
    {
    public:
        DeviceTextureCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class DeviceMultiTextureCreated : public Frameworks::IEvent
    {
    public:
        DeviceMultiTextureCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceFromMemoryCreated : public Frameworks::IEvent
    {
    public:
        TextureResourceFromMemoryCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceCreateFromMemoryFailed : public Frameworks::IEvent
    {
    public:
        TextureResourceCreateFromMemoryFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class MultiTextureResourceFromMemoryCreated : public Frameworks::IEvent
    {
    public:
        MultiTextureResourceFromMemoryCreated(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceImageLoaded : public Frameworks::IEvent
    {
    public:
        TextureResourceImageLoaded(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceLoadImageFailed : public Frameworks::IEvent
    {
    public:
        TextureResourceLoadImageFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class MultiTextureResourceImagesLoaded : public Frameworks::IEvent
    {
    public:
        MultiTextureResourceImagesLoaded(const std::string& texture_name) :
            m_textureName(texture_name) {};
        const std::string& GetTextureName() { return m_textureName; }
    private:
        std::string m_textureName;
    };
    class TextureResourceImageSaved : public Frameworks::IEvent
    {
    public:
        TextureResourceImageSaved(const std::string& texture_name, const std::string& full_filename) :
            m_textureName(texture_name), m_fullFilename(full_filename) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::string& GetFullFilename() { return m_fullFilename; }
    private:
        std::string m_textureName;
        std::string m_fullFilename;
    };
    class TextureResourceSaveImageFailed : public Frameworks::IEvent
    {
    public:
        TextureResourceSaveImageFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class MultiTextureResourceImagesSaved : public Frameworks::IEvent
    {
    public:
        MultiTextureResourceImagesSaved(const std::string& texture_name, const std::vector<std::string>& full_filenames) :
            m_textureName(texture_name), m_fullFilenames(full_filenames) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::vector<std::string>& GetFullFilenames() { return m_fullFilenames; }
    private:
        std::string m_textureName;
        std::vector<std::string>m_fullFilenames;
    };
    class TextureResourceImageRetrieved : public Frameworks::IEvent
    {
    public:
        TextureResourceImageRetrieved(const std::shared_ptr<ITexture>& target_tex, const std::string& texture_name, const MathLib::Rect& rc) :
            m_targetTexture(target_tex), m_textureName(texture_name), m_imageRect(rc) {};
        std::shared_ptr<ITexture> GetTargetTexture() const { return m_targetTexture.lock(); }
        const std::string& GetTextureName() { return m_textureName; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }
    private:
        std::weak_ptr<ITexture> m_targetTexture;
        std::string m_textureName;
        MathLib::Rect m_imageRect;
    };
    class TextureResourceRetrieveImageFailed : public Frameworks::IEvent
    {
    public:
        TextureResourceRetrieveImageFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class TextureResourceImageUpdated : public Frameworks::IEvent
    {
    public:
        TextureResourceImageUpdated(const std::shared_ptr<ITexture>& target_tex, const std::string& texture_name, const MathLib::Rect& rc) :
            m_targetTexture(target_tex), m_textureName(texture_name), m_imageRect(rc) {};
        std::shared_ptr<ITexture> GetTargetTexture() const { return m_targetTexture.lock(); }
        const std::string& GetTextureName() { return m_textureName; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }
    private:
        std::weak_ptr<ITexture> m_targetTexture;
        std::string m_textureName;
        MathLib::Rect m_imageRect;
    };
    class TextureResourceUpdateImageFailed : public Frameworks::IEvent
    {
    public:
        TextureResourceUpdateImageFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class TextureResourceAsBackSurfaceUsed : public Frameworks::IEvent
    {
    public:
        TextureResourceAsBackSurfaceUsed(const std::string& texture_name, const std::string& backsurface_name) :
            m_textureName(texture_name), m_backSurfaceName(backsurface_name) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_textureName;
        std::string m_backSurfaceName;
    };
    class TextureResourceUseAsBackSurfaceFailed : public Frameworks::IEvent
    {
    public:
        TextureResourceUseAsBackSurfaceFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& GetTextureName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class MultiTextureResourcesAsBackSurfaceUsed : public Frameworks::IEvent
    {
    public:
        MultiTextureResourcesAsBackSurfaceUsed(const std::string& texture_name, const std::string& backsurface_name) :
            m_textureName(texture_name), m_backSurfaceName(backsurface_name) {};
        const std::string& GetTextureName() { return m_textureName; }
        const std::string& GetBackSurfaceName() { return m_backSurfaceName; }
    private:
        std::string m_textureName;
        std::string m_backSurfaceName;
    };

    //---------------- Vertex/Index Buffers -----------------------//
    class DeviceVertexBufferCreated : public Frameworks::IEvent
    {
    public:
        DeviceVertexBufferCreated(const std::string& buff_name) :
            m_bufferName(buff_name) {};
        const std::string& GetBufferName() { return m_bufferName; }
    private:
        std::string m_bufferName;
    };
    class DeviceIndexBufferCreated : public Frameworks::IEvent
    {
    public:
        DeviceIndexBufferCreated(const std::string& buff_name) :
            m_bufferName(buff_name) {};
        const std::string& GetBufferName() { return m_bufferName; }
    private:
        std::string m_bufferName;
    };
    class VertexBufferResourceCreated : public Frameworks::IEvent
    {
    public:
        VertexBufferResourceCreated(const std::string& vb_name) :
            m_vertexBufferName(vb_name) {};
        const std::string& GetVertexBufferName() { return m_vertexBufferName; }
    private:
        std::string m_vertexBufferName;
    };
    class VertexBufferResourceUpdated : public Frameworks::IEvent
    {
    public:
        VertexBufferResourceUpdated(const std::string& vb_name) :
            m_vertexBufferName(vb_name) {};
        const std::string& GetVertexBufferName() { return m_vertexBufferName; }
    private:
        std::string m_vertexBufferName;
    };
    class VertexBufferResourceRangedUpdated : public Frameworks::IEvent
    {
    public:
        VertexBufferResourceRangedUpdated(const std::string& vb_name, unsigned int offset, unsigned int count) :
            m_vertexBufferName(vb_name), m_offset(offset), m_count(count) {};
        const std::string& GetVertexBufferName() { return m_vertexBufferName; }
        unsigned int GetVertexOffset() { return m_offset; }
        unsigned int GetVertexCount() { return m_count; }
    private:
        std::string m_vertexBufferName;
        unsigned int m_offset;
        unsigned int m_count;
    };
    class IndexBufferResourceCreated : public Frameworks::IEvent
    {
    public:
        IndexBufferResourceCreated(const std::string& ib_name) :
            m_indexBufferName(ib_name) {};
        const std::string& GetIndexBufferName() { return m_indexBufferName; }
    private:
        std::string m_indexBufferName;
    };
    class IndexBufferResourceUpdated : public Frameworks::IEvent
    {
    public:
        IndexBufferResourceUpdated(const std::string& ib_name) :
            m_indexBufferName(ib_name) {};
        const std::string& GetIndexBufferName() { return m_indexBufferName; }
    private:
        std::string m_indexBufferName;
    };
    class IndexBufferResourceRangedUpdated : public Frameworks::IEvent
    {
    public:
        IndexBufferResourceRangedUpdated(const std::string& ib_name, unsigned int offset, unsigned int count) :
            m_indexBufferName(ib_name), m_offset(offset), m_count(count) {};
        const std::string& GetIndexBufferName() { return m_indexBufferName; }
        unsigned int GetIndexOffset() { return m_offset; }
        unsigned int GetIndexCount() { return m_count; }
    private:
        std::string m_indexBufferName;
        unsigned int m_offset;
        unsigned int m_count;
    };
    class VertexBufferUpdateFailed : public Frameworks::IEvent
    {
    public:
        VertexBufferUpdateFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& getName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class IndexBufferUpdateFailed : public Frameworks::IEvent
    {
    public:
        IndexBufferUpdateFailed(const std::string& name, std::error_code err) :
            m_name(name), m_error(err) {};
        const std::string& getName() const { return m_name; }
        std::error_code GetError() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    //---------------- Shaders -----------------------//
    class DeviceVertexShaderCreated : public Frameworks::IEvent
    {
    public:
        DeviceVertexShaderCreated(const std::string& name) :
            m_name(name) {};
        const std::string& getName() { return m_name; }
    private:
        std::string m_name;
    };
    class DevicePixelShaderCreated : public Frameworks::IEvent
    {
    public:
        DevicePixelShaderCreated(const std::string& name) :
            m_name(name) {};
        const std::string& getName() { return m_name; }
    private:
        std::string m_name;
    };
    class DeviceShaderProgramCreated : public Frameworks::IEvent
    {
    public:
        DeviceShaderProgramCreated(const std::string& name) :
            m_name(name) {};
        const std::string& getName() { return m_name; }
    private:
        std::string m_name;
    };
    class DeviceVertexDeclarationCreated : public Frameworks::IEvent
    {
    public:
        DeviceVertexDeclarationCreated(const std::string& name) :
            m_name(name) {};
        const std::string& getName() { return m_name; }
    private:
        std::string m_name;
    };
    class VertexShaderCompiled : public Frameworks::IEvent
    {
    public:
        VertexShaderCompiled(const std::string& name) :
            m_name(name) {};
        const std::string& GetShaderName() { return m_name; }
    private:
        std::string m_name;
    };
    class PixelShaderCompiled : public Frameworks::IEvent
    {
    public:
        PixelShaderCompiled(const std::string& name) :
            m_name(name) {};
        const std::string& GetShaderName() { return m_name; }
    private:
        std::string m_name;
    };
    class VertexShaderCompileFailed : public Frameworks::IEvent
    {
    public:
        VertexShaderCompileFailed(const std::string& name, const std::string& err) :
            m_name(name), m_error(err) {};
        const std::string& GetShaderName() { return m_name; }
        const std::string& GetError() { return m_error; }
    private:
        std::string m_name;
        std::string m_error;
    };
    class PixelShaderCompileFailed : public Frameworks::IEvent
    {
    public:
        PixelShaderCompileFailed(const std::string& name, const std::string& err) :
            m_name(name), m_error(err) {};
        const std::string& GetShaderName() { return m_name; }
        const std::string& GetError() { return m_error; }
    private:
        std::string m_name;
        std::string m_error;
    };
    class ShaderProgramLinkFailed : public Frameworks::IEvent
    {
    public:
        ShaderProgramLinkFailed(const std::string& name, const std::string& err) :
            m_name(name), m_error(err) {};
        const std::string& GetShaderName() { return m_name; }
        const std::string& GetError() { return m_error; }
    private:
        std::string m_name;
        std::string m_error;
    };
    class DeviceInputLayoutCreated : public Frameworks::IEvent
    {
    public:
        DeviceInputLayoutCreated(const std::string& name) :
            m_name(name) {};
        const std::string& GetVertexDeclarationName() { return m_name; }
    private:
        std::string m_name;
    };
    class ShaderVariableCreated : public Frameworks::IEvent
    {
    public:
        enum class VarType
        {
            constBuffer,
            floatValue,
            intValue,
            booleanValue,
            matrixValue,
            vectorValue,
            textureObject,
            samplerState
        };
    public:
        ShaderVariableCreated(const std::string& name, const std::string& semantic, VarType type) :
            m_name(name), m_semantic(semantic), m_type(type) {};
        const std::string& GetVariableName() { return m_name; }
        const std::string& GetVariableSemantic() { return m_semantic; }
        VarType GetVariableType() const { return m_type; }
    private:
        std::string m_name;
        std::string m_semantic;
        VarType m_type;
    };

    //---------------- Device States -----------------------//
    class DeviceSamplerStateCreated : public Frameworks::IEvent
    {
    public:
        DeviceSamplerStateCreated(const std::string& name, const std::shared_ptr<IDeviceSamplerState>& state) :
            m_stateName(name), m_state(state) {};
        const std::string& GetStateName() { return m_stateName; }
        const std::shared_ptr<IDeviceSamplerState>& GetState() const { return m_state; }
    private:
        std::string m_stateName;
        std::shared_ptr<IDeviceSamplerState> m_state;
    };
    class DeviceAlphaBlendStateCreated : public Frameworks::IEvent
    {
    public:
        DeviceAlphaBlendStateCreated(const std::string& name, const std::shared_ptr<IDeviceAlphaBlendState>& state) :
            m_stateName(name), m_state(state) {};
        const std::string& GetStateName() { return m_stateName; }
        const std::shared_ptr<IDeviceAlphaBlendState>& GetState() const { return m_state; }
    private:
        std::string m_stateName;
        std::shared_ptr<IDeviceAlphaBlendState> m_state;
    };
    class DeviceDepthStencilStateCreated : public Frameworks::IEvent
    {
    public:
        DeviceDepthStencilStateCreated(const std::string& name, const std::shared_ptr<IDeviceDepthStencilState>& state) :
            m_stateName(name), m_state(state) {};
        const std::string& GetStateName() { return m_stateName; }
        const std::shared_ptr<IDeviceDepthStencilState>& GetState() const { return m_state; }
    private:
        std::string m_stateName;
        std::shared_ptr<IDeviceDepthStencilState> m_state;
    };
    class DeviceRasterizerStateCreated : public Frameworks::IEvent
    {
    public:
        DeviceRasterizerStateCreated(const std::string& name, const std::shared_ptr<IDeviceRasterizerState>& state) :
            m_stateName(name), m_state(state) {};
        const std::string& GetStateName() { return m_stateName; }
        const std::shared_ptr<IDeviceRasterizerState>& GetState() const { return m_state; }
    private:
        std::string m_stateName;
        std::shared_ptr<IDeviceRasterizerState> m_state;
    };
    class ConstBufferResourceCreated : public Frameworks::IEvent
    {
    public:
        ConstBufferResourceCreated(const std::string& name) :
            m_bufferName(name) {};
        const std::string& GetBufferName() { return m_bufferName; }
    private:
        std::string m_bufferName;
    };
    class ConstBufferResourceApplied : public Frameworks::IEvent
    {
    public:
        ConstBufferResourceApplied(const std::string& name) :
            m_bufferName(name) {};
        const std::string& GetBufferName() { return m_bufferName; }
    private:
        std::string m_bufferName;
    };
    class SamplerStateResourceCreated : public Frameworks::IEvent
    {
    public:
        SamplerStateResourceCreated(const std::string& name) :
            m_stateName(name) {};
        const std::string& GetStateName() { return m_stateName; }
    private:
        std::string m_stateName;
    };
    class AlphaBlendStateResourceCreated : public Frameworks::IEvent
    {
    public:
        AlphaBlendStateResourceCreated(const std::string& name) :
            m_stateName(name) {};
        const std::string& GetStateName() { return m_stateName; }
    private:
        std::string m_stateName;
    };
    class RasterizerStateResourceCreated : public Frameworks::IEvent
    {
    public:
        RasterizerStateResourceCreated(const std::string& name) :
            m_stateName(name) {};
        const std::string& GetStateName() { return m_stateName; }
    private:
        std::string m_stateName;
    };
    class DepthStencilStateResourceCreated : public Frameworks::IEvent
    {
    public:
        DepthStencilStateResourceCreated(const std::string& name) :
            m_stateName(name) {};
        const std::string& GetStateName() { return m_stateName; }
    private:
        std::string m_stateName;
    };

}

#endif // GRAPHIC_EVENTS_H
