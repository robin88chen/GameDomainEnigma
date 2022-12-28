/*********************************************************************
 * \file   ModelPrimitiveMaker.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_MAKER_H
#define _MODEL_PRIMITIVE_MAKER_H

#include <memory>
#include "Renderer/RenderablePrimitivePolicies.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"

class ModelPrimitiveMaker
{
public:
    static std::shared_ptr<Enigma::Renderer::ModelPrimitivePolicy> MakeModelPrimitivePolicy(const std::string& model_name, const std::string& geo_name);

private:
    static Enigma::Renderer::MeshPrimitiveDto MakeMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name);
    static Enigma::Engine::EffectMaterialDto MakeEffectDto(const std::string& eff_name);
    static Enigma::Engine::EffectTextureMapDto MakeTextureMapDto();
};

#endif // _MODEL_PRIMITIVE_MAKER_H
