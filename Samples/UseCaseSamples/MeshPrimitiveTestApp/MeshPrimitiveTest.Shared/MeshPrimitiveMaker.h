/*********************************************************************
 * \file   MeshPrimitiveMaker.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_MAKER_H
#define _MESH_PRIMITIVE_MAKER_H

#include "Renderer/RenderablePrimitivePolicies.h"
#include "GameEngine/EffectMaterialPolicy.h"
#include "GameEngine/EffectTextureMapPolicy.h"
#include "Renderer/MeshPrimitive.h"
#include <memory>

class MeshPrimitiveMaker
{
public:
    static std::shared_ptr<Enigma::Renderer::MeshPrimitivePolicy> MakeMeshPrimitivePolicy(const std::string& mesh_name, const std::string& geo_name);
    static void SaveMeshPrimitiveDto(const std::shared_ptr<Enigma::Renderer::MeshPrimitive>& mesh, const std::string& filename_at_path);
    static std::shared_ptr<Enigma::Renderer::MeshPrimitivePolicy> LoadMeshPrimitivePolicy(const std::string& filename_at_path);

private:
    static Enigma::Engine::EffectMaterialPolicy MakeEffectPolicy(const std::string& eff_name);
    static Enigma::Engine::EffectTextureMapPolicy MakeTextureMapPolicy();
};

#endif // _MESH_PRIMITIVE_MAKER_H
