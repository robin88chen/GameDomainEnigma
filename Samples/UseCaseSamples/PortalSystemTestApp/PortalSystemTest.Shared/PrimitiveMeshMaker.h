/*********************************************************************
 * \file   PrimitiveMeshMaker.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PRIMITIVE_MESH_MAKER_H
#define _PRIMITIVE_MESH_MAKER_H
#include <string>
#include <GameEngine/BoundingVolume.h>
#include <GameEngine/EffectMaterialDto.h>
#include <GameEngine/EffectTextureMapDto.h>
#include <Renderer/RenderablePrimitiveDtos.h>

class PrimitiveMeshMaker
{
public:
    static void MakeSavedFloorGeometry(const std::string& name);
    static void MakeSavedDoorGeometry(const std::string& name);
    static Enigma::Renderer::MeshPrimitiveDto MakeMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name,
        const std::string& eff_name, const std::string& eff_filename,
        const std::string& tex_filename, const std::string& tex_name, const std::string& tex_semantic);

    static const Enigma::Engine::BoundingVolume& GetFloorBound() { return m_floorBounding; }
    static const Enigma::Engine::BoundingVolume& GetDoorBound() { return m_doorBounding; }

private:
    static Enigma::Engine::EffectMaterialDto MakeEffectDto(const std::string& eff_name, const std::string& eff_filename);
    static Enigma::Engine::EffectTextureMapDto MakeTextureMapDto(const std::string& filename, const std::string& tex_name, const std::string& semantic);

private:
    static Enigma::Engine::BoundingVolume m_floorBounding;
    static Enigma::Engine::BoundingVolume m_doorBounding;
};

#endif // _PRIMITIVE_MESH_MAKER_H
