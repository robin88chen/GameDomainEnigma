/*********************************************************************
 * \file   IDeviceRasterizerState.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_RASTERIZER_STATE_INTERFACE_H
#define DEVICE_RASTERIZER_STATE_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <string>
#include <memory>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IDeviceRasterizerState : public std::enable_shared_from_this<IDeviceRasterizerState>
    {
    public:
        struct RasterizerStateData
        {
            enum class BackfaceCullMode : unsigned char
            {
                Cull_None = 0,
                Cull_CCW,
                Cull_CW,
            };
            enum class FillMode : unsigned char
            {
                Fill,
                Wireframe,
                Point,
            };
            BackfaceCullMode m_cullMode;
            FillMode m_fillMode;
            float m_depthBias;
            float m_depthBiasClamp;
            float m_slopeScaledDepthBias;
            RasterizerStateData() :  m_cullMode{ BackfaceCullMode::Cull_CCW }, m_fillMode{ FillMode::Fill },
                m_depthBias{ 0 }, m_depthBiasClamp{ 0.0f }, m_slopeScaledDepthBias{ 0.0f }
            {};
        };
    public:
        IDeviceRasterizerState(const std::string& name);
        IDeviceRasterizerState(const IDeviceRasterizerState&) = delete;
        IDeviceRasterizerState(IDeviceRasterizerState&&) = delete;
        virtual ~IDeviceRasterizerState();
        IDeviceRasterizerState& operator=(const IDeviceRasterizerState&) = delete;
        IDeviceRasterizerState& operator=(IDeviceRasterizerState&&) = delete;

        //virtual void create(const RasterizerStateData& data);
        virtual error CreateFromData(const RasterizerStateData& data);
        virtual future_error AsyncCreateFromData(const RasterizerStateData& data);
        virtual void Bind();

    protected:
        virtual error BindToDevice() = 0;
        virtual future_error AsyncBindToDevice();

    protected:
        std::string m_name;
        RasterizerStateData m_data;
    };
    using IDeviceRasterizerStatePtr = std::shared_ptr<IDeviceRasterizerState>;
}


#endif // DEVICE_RASTERIZER_STATE_INTERFACE_H
