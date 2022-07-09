/*********************************************************************
 * \file   IDeviceSamplerState.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DEVICE_SAMPLER_STATE_INTERFACE_H
#define DEVICE_SAMPLER_STATE_INTERFACE_H

#include "MathLib/ColorRGBA.h"
#include "MathLib/MathGlobal.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <memory>
#include <string>

namespace Enigma::Graphics
{
    using error = std::error_code;
    class IDeviceSamplerState : public std::enable_shared_from_this<IDeviceSamplerState>
    {
    public:
        struct SamplerStateData
        {
            enum class Filter : unsigned char
            {
                None, // only for mip filter
                Point,
                Linear
            };
            enum class AddressMode : unsigned char
            {
                Wrap,
                Mirror,
                Clamp,
                Border
            };
            enum class CompareFunc : unsigned char
            {
                Never = 1,
                Less,
                Equal,
                LessEqual,
                Greater,
                NotEqual,
                GreaterEqual,
                Always
            };
            Filter m_minFilter;
            Filter m_magFilter;
            Filter m_mipFilter;
            CompareFunc m_cmpFunc;
            AddressMode m_addressModeU;
            AddressMode m_addressModeV;
            AddressMode m_addressModeW;
            unsigned char m_levelAnisotropic; // 0 : no, level : 1 ~ 16
            float m_mipmapLODBias;
            MathLib::ColorRGBA m_borderColor;
            float m_minLOD;
            float m_maxLOD;
            SamplerStateData() : m_minFilter(Filter::Linear), m_magFilter(Filter::Linear), m_mipFilter(Filter::Linear),
                m_cmpFunc(CompareFunc::Never),
                m_addressModeU(AddressMode::Clamp), m_addressModeV(AddressMode::Clamp), m_addressModeW(AddressMode::Clamp),
                m_levelAnisotropic(0), m_mipmapLODBias(0.0f), m_borderColor(1.0f, 1.0f, 1.0f, 1.0f),
                m_minLOD(-MathLib::Math::MAX_FLOAT), m_maxLOD(MathLib::Math::MAX_FLOAT) {};
        };
    public:
        IDeviceSamplerState(const std::string& name);
        IDeviceSamplerState(const IDeviceSamplerState&) = delete;
        IDeviceSamplerState(IDeviceSamplerState&&) = delete;
        virtual ~IDeviceSamplerState();
        IDeviceSamplerState& operator=(const IDeviceSamplerState&) = delete;
        IDeviceSamplerState& operator=(IDeviceSamplerState&&) = delete;

        virtual error CreateFromData(const SamplerStateData& data);
        virtual future_error AsyncCreateFromData(const SamplerStateData& data);

    protected:
        std::string m_name;
        SamplerStateData m_data;
    };
    using IDeviceSamplerStatePtr = std::shared_ptr<IDeviceSamplerState>;
    using IDeviceSamplerStateWeak = std::weak_ptr<IDeviceSamplerState>;
}

#endif // DEVICE_SAMPLER_STATE_INTERFACE_H
