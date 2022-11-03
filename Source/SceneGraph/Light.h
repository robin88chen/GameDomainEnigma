/*********************************************************************
 * \file   Light.h
 * \brief  Spatial Light, entity, maintained by repository
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef LIGHT_H
#define LIGHT_H

#include "Spatial.h"
#include "MathLib/Matrix4.h"
#include "LightInfo.h"
#include "Frameworks/Rtti.h"
#include <memory>
#include <system_error>

namespace Enigma::SceneGraph
{
    using error = std::error_code;
    class LightContract;

    class Light : public Spatial
    {
        DECLARE_EN_RTTI
    public:
        Light(const std::string& spatialName, const LightInfo& lightInfo);
        Light(const LightContract& contract);
        Light(const Light&) = delete;
        Light(Light&&) = delete;
        virtual ~Light() override;
        Light& operator=(const Light&) = delete;
        Light& operator=(Light&&) = delete;

        LightContract SerializeContract();

        const LightInfo& Info() const { return m_lightInfo; };
        LightInfo& Info() { return m_lightInfo; }

        virtual error OnCullingVisible(Culler*, bool) override;
        virtual bool CanVisited() override { return true; };
        virtual error _UpdateWorldData(const MathLib::Matrix4& mxParentWorld) override;

        void SetLightColor(const MathLib::ColorRGBA& color);
        const MathLib::ColorRGBA& GetLightColor() { return Info().GetLightColor(); };

        void SetLightPosition(const MathLib::Vector3& vec);
        const MathLib::Vector3& GetLightPosition() { return Info().GetLightPosition(); };

        void SetLightDirection(const MathLib::Vector3& vec);
        const MathLib::Vector3& GetLightDirection() { return Info().GetLightDirection(); };

        void SetLightRange(float range);
        float GetLightRange() { return Info().GetLightRange(); };

        /// x,y,z : 0次,1次,2次係數
        void SetLightAttenuation(const MathLib::Vector3& vecAttenuation);
        const MathLib::Vector3& GetLightAttenuation() { return Info().GetLightAttenuation(); };

        void SetEnable(bool flag);
        bool IsEnable() { return Info().IsEnable(); };

    protected:
        std::shared_ptr<Light> ThisLight()
        {
            return std::dynamic_pointer_cast<Light, Spatial>(shared_from_this());
        }
        std::shared_ptr<const Light> ThisLight() const
        {
            return std::dynamic_pointer_cast<const Light, const Spatial>(shared_from_this());
        }

    protected:
        LightInfo m_lightInfo;
    };
}

#endif // LIGHT_H
