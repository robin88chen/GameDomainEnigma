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

    class Light : public Spatial
    {
        DECLARE_EN_RTTI
    public:
        Light(const SpatialId& id, const LightInfo& lightInfo);
        Light(const Light&) = delete;
        Light(Light&&) = delete;
        virtual ~Light() override;
        Light& operator=(const Light&) = delete;
        Light& operator=(Light&&) = delete;

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler) override;

        const LightInfo& info() const { return m_lightInfo; };
        LightInfo& info() { return m_lightInfo; }

        virtual error onCullingVisible(Culler*, bool) override;
        virtual bool canVisited() override { return true; };
        virtual error _updateWorldData(const MathLib::Matrix4& mxParentWorld) override;

        void setLightColor(const MathLib::ColorRGBA& color);
        const MathLib::ColorRGBA& getLightColor() { return info().color(); };

        void setLightPosition(const MathLib::Vector3& vec);
        const MathLib::Vector3& getLightPosition() { return info().position(); };

        void setLightDirection(const MathLib::Vector3& vec);
        const MathLib::Vector3& getLightDirection() { return info().direction(); };

        void setLightRange(float range);
        float getLightRange() { return info().range(); };

        /// x,y,z : 0次,1次,2次係數
        void setLightAttenuation(const MathLib::Vector3& vecAttenuation);
        const MathLib::Vector3& getLightAttenuation() { return info().attenuation(); };

        void setEnable(bool flag);
        bool isEnable() { return info().isEnable(); };

    protected:
        std::shared_ptr<Light> thisLight()
        {
            return std::dynamic_pointer_cast<Light, Spatial>(shared_from_this());
        }
        std::shared_ptr<const Light> thisLight() const
        {
            return std::dynamic_pointer_cast<const Light, const Spatial>(shared_from_this());
        }

    protected:
        LightInfo m_lightInfo;
    };
}

#endif // LIGHT_H
