/*****************************************************************
 * \file   LightAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef LIGHT_ASSEMBLERS_H
#define LIGHT_ASSEMBLERS_H

#include "LightDtos.h"
#include "SpatialId.h"
#include "SpatialAssembler.h"

namespace Enigma::SceneGraph
{
    class Light;

    class LightAssembler
    {
    public:
        LightAssembler(const SpatialId& id, const LightInfo::LightType& type);

        const SpatialId& id() const { return m_id; }

        SpatialAssembler& spatial();

        LightAssembler& factory(const Engine::FactoryDesc& factory);
        LightAssembler& asNative(const std::string& file_at_path);
        LightAssembler& color(const MathLib::ColorRGBA& color);
        LightAssembler& position(const MathLib::Vector3& position);
        LightAssembler& direction(const MathLib::Vector3& direction);
        LightAssembler& range(float range);
        LightAssembler& attenuation(const MathLib::Vector3& attenuation);
        LightAssembler& isEnable(bool is_enable);

        LightDto toLightDto() const;
        Engine::GenericDto toGenericDto() const;

        std::shared_ptr<Light> constitute();

    private:
        SpatialId m_id;
        Engine::FactoryDesc m_factory;
        SpatialAssembler m_spatialAssembler;
        LightInfoDto m_info;
    };
}

#endif // LIGHT_ASSEMBLERS_H
