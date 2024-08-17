#include "ModelAnimationDtos.h"
#include "ModelAnimationAsset.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;

static std::string TOKEN_SCALE_TIME_KEYS = "ScaleTimeKeys";
static std::string TOKEN_ROTATE_TIME_KEYS = "RotateTimeKeys";
static std::string TOKEN_TRANSLATE_TIME_KEYS = "TranslateTimeKeys";
static std::string TOKEN_ID = "Id";
static std::string TOKEN_MESH_NODE_NAMES = "MeshNodeNames";
static std::string TOKEN_TIME_SRTS = "TimeSRTs";

AnimationTimeSRTDto::AnimationTimeSRTDto(const GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<std::vector<float>>(TOKEN_SCALE_TIME_KEYS)) scaleTimeKeys() = v.value();
    if (const auto v = dto.tryGetValue<std::vector<float>>(TOKEN_ROTATE_TIME_KEYS)) rotateTimeKeys() = v.value();
    if (const auto v = dto.tryGetValue<std::vector<float>>(TOKEN_TRANSLATE_TIME_KEYS)) translateTimeKeys() = v.value();
}

GenericDto AnimationTimeSRTDto::toGenericDto()
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_SCALE_TIME_KEYS, m_scaleTimeKeys);
    dto.addOrUpdate(TOKEN_ROTATE_TIME_KEYS, m_rotateTimeKeys);
    dto.addOrUpdate(TOKEN_TRANSLATE_TIME_KEYS, m_translateTimeKeys);
    return dto;
}


ModelAnimationAssetDto::ModelAnimationAssetDto() //: AnimationAssetDto()
{
    //m_factoryDesc = FactoryDesc(ModelAnimationAsset::TYPE_RTTI.getName());
}

ModelAnimationAssetDto::ModelAnimationAssetDto(const GenericDto& dto) //: AnimationAssetDto()
{
    //m_factoryDesc = dto.getRtti();
    //if (const auto v = dto.tryGetValue<std::string>(TOKEN_ID)) id(v.value());
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_NODE_NAMES)) meshNodeNames() = v.value();
    if (const auto v = dto.tryGetValue<GenericDtoCollection>(TOKEN_TIME_SRTS)) timeSRTs() = v.value();
}

GenericDto ModelAnimationAssetDto::toGenericDto()
{
    GenericDto dto;
    //dto.addRtti(m_factoryDesc);
    //dto.addOrUpdate(TOKEN_ID, m_id.name());
    dto.addOrUpdate(TOKEN_MESH_NODE_NAMES, m_meshNodeNames);
    dto.addOrUpdate(TOKEN_TIME_SRTS, m_timeSrtDtos);
    return dto;
}
