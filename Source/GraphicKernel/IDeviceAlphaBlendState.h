/*********************************************************************
 * \file   IDeviceAlphaBlendState.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_ALPHA_BLEND_STATE_INTERFACE_H
#define DEVICE_ALPHA_BLEND_STATE_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include <string>
#include <memory>

#define MAX_BLEND_TARGET 8

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IDeviceAlphaBlendState : public std::enable_shared_from_this<IDeviceAlphaBlendState>
    {
    public:
        struct BlendStateData
        {
        public:
            enum class BlendType
            {
                Blend_Disable = 0,
                Blend_Opaque, ///< 不透明, src = one, dest = zero
                Blend_Additive,		///< 相加的blend, src color + dest color
                Blend_Transparent,	///< 半透明的blend, src color * alpha + dest color * inv_alpha
            };
            bool m_isMultiBlendTarget;
            BlendType m_blendType[MAX_BLEND_TARGET] =
            { BlendType::Blend_Disable, BlendType::Blend_Disable,
                BlendType::Blend_Disable, BlendType::Blend_Disable,
                BlendType::Blend_Disable, BlendType::Blend_Disable,
                BlendType::Blend_Disable, BlendType::Blend_Disable
            };
            BlendStateData() : m_isMultiBlendTarget(false) {};
        };
    public:
        IDeviceAlphaBlendState(const std::string& name);
        IDeviceAlphaBlendState(const IDeviceAlphaBlendState&) = delete;
        IDeviceAlphaBlendState(IDeviceAlphaBlendState&&) = delete;
        virtual ~IDeviceAlphaBlendState();
        IDeviceAlphaBlendState& operator=(const IDeviceAlphaBlendState&) = delete;
        IDeviceAlphaBlendState& operator=(IDeviceAlphaBlendState&&) = delete;

        virtual void Create(const BlendStateData& data);
        virtual void Bind();

    protected:
        virtual error CreateFromData(const BlendStateData& data);
        virtual future_error AsyncCreateFromData(const BlendStateData& data);
        virtual error BindToDevice() = 0;
        virtual future_error AsyncBindToDevice();

    protected:
        std::string m_name;
        BlendStateData m_data;
    };
    using IDeviceAlphaBlendStatePtr = std::shared_ptr<IDeviceAlphaBlendState>;
}


#endif // DEVICE_ALPHA_BLEND_STATE_INTERFACE_H
