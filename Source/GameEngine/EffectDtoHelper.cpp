#include "EffectDtoHelper.h"
#include "EffectMaterial.h"
#include "FileSystem/Filename.h"

using namespace Enigma::Engine;

/*EffectMaterialDtoHelper::EffectMaterialDtoHelper(const std::string& name)
{
    m_dto.id() = name;
}

EffectMaterialDtoHelper& EffectMaterialDtoHelper::FilenameAtPath(const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.getName()).ClaimFromResource(m_dto.id().name(), filename, path_id);
    return *this;
}

EffectMaterialDtoHelper& EffectMaterialDtoHelper::FilenameAtPath(const std::string& filename_at_path)
{
    FileSystem::Filename fn(filename_at_path);
    return FilenameAtPath(fn.getSubPathFileName(), fn.getMountPathId());
}

GenericDto EffectMaterialDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}*/

EffectTextureMapDtoHelper::EffectTextureMapDtoHelper()
{
}

EffectTextureMapDtoHelper& EffectTextureMapDtoHelper::textureMapping(const TextureId& id, std::optional<unsigned> array_index, const std::string& semantic)
{
    TextureMappingDto tex;
    tex.textureId() = id;
    tex.semantic() = semantic;
    if (array_index) tex.arrayIndex() = array_index.value();
    m_dto.textureMappings().emplace_back(tex);
    return *this;
}

GenericDto EffectTextureMapDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}
