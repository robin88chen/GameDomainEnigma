/*********************************************************************
 * \file   ModelPrimitiveMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_MAKER_H
#define _MODEL_PRIMITIVE_MAKER_H

#include "GameEngine/PrimitiveId.h"
#include "Geometries/GeometryId.h"

class ModelPrimitiveMaker
{
public:
    static void makeModelPrimitive(const Enigma::Engine::PrimitiveId& model_id, const Enigma::Engine::PrimitiveId& mesh_id);
    static void makeCubeMeshPrimitive(const Enigma::Engine::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id);
    //static void SaveModelPrimitiveDto(const std::shared_ptr<Enigma::Renderer::ModelPrimitive>& model, const std::string& filename_at_path);
    //static std::shared_ptr<Enigma::Renderer::ModelPrimitivePolicy> LoadModelPrimitivePolicy(const std::string& filename_at_path);

private:
    //static Enigma::Renderer::MeshPrimitiveDto MakeMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name);
    //static Enigma::Engine::EffectMaterialDto MakeEffectDto(const std::string& eff_name);
    //static Enigma::Engine::EffectTextureMapDto MakeTextureMapDto();
};

#endif // _MODEL_PRIMITIVE_MAKER_H
