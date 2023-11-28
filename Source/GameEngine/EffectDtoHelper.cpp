#include "EffectDtoHelper.h"
#include "EffectMaterial.h"
#include "FileSystem/Filename.h"

using namespace Enigma::Engine;

EffectMaterialDtoHelper::EffectMaterialDtoHelper(const std::string& name)
{
    m_dto.Name() = name;
}

EffectMaterialDtoHelper& EffectMaterialDtoHelper::FilenameAtPath(const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.getName()).ClaimFromResource(m_dto.Name(), filename, path_id);
    return *this;
}

EffectMaterialDtoHelper& EffectMaterialDtoHelper::FilenameAtPath(const std::string& filename_at_path)
{
    FileSystem::Filename fn(filename_at_path);
    return FilenameAtPath(fn.GetSubPathFileName(), fn.GetMountPathID());
}

GenericDto EffectMaterialDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

EffectTextureMapDtoHelper::EffectTextureMapDtoHelper()
{
}

EffectTextureMapDtoHelper& EffectTextureMapDtoHelper::TextureMapping(const std::string& filename, const std::string& path_id,
       const std::string& tex_name, std::optional<unsigned> array_index, const std::string& semantic)
{
    TextureMappingDto tex;
    tex.JobType() = TexturePolicy::JobType::Load;
    tex.Filename() = filename;
    tex.Semantic() = semantic;
    tex.TextureName() = tex_name;
    tex.PathId() = path_id;
    if (array_index) tex.ArrayIndex() = array_index.value();
    m_dto.TextureMappings().emplace_back(tex);
    return *this;
}

EffectTextureMapDtoHelper& EffectTextureMapDtoHelper::TextureMapping(const MathLib::Dimension<unsigned>& dimension, 
    unsigned surface_count, const std::string& tex_name, std::optional<unsigned> array_index, const std::string& semantic)
{
    TextureMappingDto tex;
    tex.JobType() = TexturePolicy::JobType::Create;
    tex.Dimension() = dimension;
    tex.SurfaceCount() = surface_count;
    tex.Semantic() = semantic;
    tex.TextureName() = tex_name;
    if (array_index) tex.ArrayIndex() = array_index.value();
    m_dto.TextureMappings().emplace_back(tex);
    return *this;
}

GenericDto EffectTextureMapDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}
