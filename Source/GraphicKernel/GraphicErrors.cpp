#include "GraphicErrors.h"

using namespace Enigma::Graphics;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::fileIO: return "File IO error";
    case ErrorCode::notImplement: return "not implement yet";
    case ErrorCode::nullMemoryBuffer: return "null memory buffer";
    case ErrorCode::invalidParameter: return "invalid parameter";
    case ErrorCode::dxgiInitialize: return "DXGI initialize state fail";
    case ErrorCode::invalidWindow: return "Invalid window";
    case ErrorCode::adapterFail: return "Adapter fail";
    case ErrorCode::enumerateDevicesFail: return "Enumerate devices fail";
    case ErrorCode::resizeBackSurfaceFail: return "Resize back surface fail";
    case ErrorCode::createDeviceFail: return "Create device fail";
    case ErrorCode::d3dDeviceNullPointer: return "D3D Device null pointer";
    case ErrorCode::swapChain: return "Swap Chain fail";
    case ErrorCode::createBackSurfaceFail: return "Create back surface fail";
    case ErrorCode::createDepthSurfaceFail: return "Create depth surface fail";
    case ErrorCode::dynamicCastSurface: return "Dynamic cast to device surface fail";
    case ErrorCode::nullBackSurface: return "null back surface";
    case ErrorCode::deviceCreateTexture: return "Device create texture fail";
    case ErrorCode::dxLoadTexture: return "Dx Load Texture fail";
    case ErrorCode::dxCreateTexture: return "Dx Create texture fail";
    case ErrorCode::dxTextureMapping: return "Dx texture mapping";
    case ErrorCode::nullDxTexture: return "Null Dx Texture";
    case ErrorCode::dxSaveTexture: return "Dx Save texture fail";
    case ErrorCode::saveTextureFile: return "save texture file fail";
    case ErrorCode::dxCreateShaderResource: return "Dx create shader resource";
    case ErrorCode::deviceCreateVertexBuffer: return "Device create vertex buffer fail";
    case ErrorCode::deviceCreateIndexBuffer: return "Device create index buffer fail";
    case ErrorCode::bufferSize: return "buffer size error";
    case ErrorCode::deviceCreateVertexShader: return "device create vertex shader fail";
    case ErrorCode::deviceCreatePixelShader: return "device create pixel shader fail";
    case ErrorCode::compileShader: return "compile shader fail";
    case ErrorCode::dxCreateShader: return "Dx create shader fail";
    case ErrorCode::shaderSignature: return "Shader signature error";
    case ErrorCode::vertexLayout: return "vertex layout error";
    case ErrorCode::dxCreateInputLayout: return "Dx create input layout fail";
    case ErrorCode::nullSamplerState: return "null sampler state";
    case ErrorCode::dynamicCastState: return "dynamic cast state fail";
    case ErrorCode::applyTextureVariable: return "apply texture variable fail";
    case ErrorCode::duplicatedVertexDeclaration: return "duplicated vertex declaration name";
    case ErrorCode::dxCreateBuffer: return "Dx create buffer fail";
    case ErrorCode::dxBufferMapping: return "Dx buffer mapping fail";
    case ErrorCode::dxCreateSamplerState: return "Dx create sampler state fail";
    case ErrorCode::dxCreateBlendState: return "Dx create alpha blend state fail";
    case ErrorCode::dxCreateRasterizerState: return "Dx create rasterizer state fail";
    case ErrorCode::dxCreateDepthState: return "Dx create depth stencil state fail";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Graphics Error Category";
}
namespace Enigma::Graphics
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}
