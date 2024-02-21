/*********************************************************************
 * \file   IDeviceDepthStencilState.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_DEPTH_STENCIL_STATE_INTERFACE_H
#define DEVICE_DEPTH_STENCIL_STATE_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <string>
#include <memory>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IDeviceDepthStencilState : public std::enable_shared_from_this<IDeviceDepthStencilState>
    {
    public:
        struct DepthStencilData
        {
        public:
            enum class CompareFunc
            {
                Never = 0,
                Less,
                Equal,
                LessEqual,
                Greater,
                NotEqual,
                GreaterEqual,
                Always,
            };
            enum class StencilOpCode
            {
                Keep = 1,
                Zero,
                Replace,
                Incr_Sat,
                Decr_Sat,
                Invert,
                Incr,
                Decr,
            };
            struct StencilOpData
            {
                CompareFunc m_compare;
                StencilOpCode m_failOp;
                StencilOpCode m_passOp;
                StencilOpCode m_depthFailOp;
                StencilOpData() : m_compare{ CompareFunc::Always },
                    m_failOp{ StencilOpCode::Keep }, m_passOp{ StencilOpCode::Keep },
                    m_depthFailOp{ StencilOpCode::Keep } {};
            };
            bool m_isDepthTestEnable;
            bool m_isDepthWriteEnable;
            CompareFunc m_depthCompare;
            bool m_isStencilTestEnable;
            unsigned char m_stencilWriteMask;
            unsigned char m_stencilReadMask;
            StencilOpData m_frontFaceOp;
            StencilOpData m_backFaceOp;
            unsigned int m_stencilRefValue;

            DepthStencilData() : m_isDepthTestEnable(true), m_isDepthWriteEnable(true),
                m_depthCompare{ CompareFunc::Less }, m_isStencilTestEnable{ false },
                m_stencilWriteMask{ 0xff }, m_stencilReadMask{ 0xff }, m_stencilRefValue{ 0 } {};
        };
    public:
        IDeviceDepthStencilState(const std::string& name);
        IDeviceDepthStencilState(const IDeviceDepthStencilState&) = delete;
        IDeviceDepthStencilState(IDeviceDepthStencilState&&) = delete;
        virtual ~IDeviceDepthStencilState();
        IDeviceDepthStencilState& operator=(const IDeviceDepthStencilState&) = delete;
        IDeviceDepthStencilState& operator=(IDeviceDepthStencilState&&) = delete;

        //virtual void create(const DepthStencilData& data);
        virtual error CreateFromData(const DepthStencilData& data);
        virtual future_error AsyncCreateFromData(const DepthStencilData& data);
        virtual void bind();

    protected:
        virtual error BindToDevice() = 0;
        virtual future_error AsyncBindToDevice();

    protected:
        std::string m_name;
        DepthStencilData m_data;
    };
    using IDeviceDepthStencilStatePtr = std::shared_ptr<IDeviceDepthStencilState>;
}


#endif // DEVICE_DEPTH_STENCIL_STATE_INTERFACE_H
