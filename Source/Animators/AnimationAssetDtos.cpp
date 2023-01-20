#include "AnimationAssetDtos.h"
#include "ModelAnimationAsset.h"

using namespace Enigma::Animators;
using namespace Enigma::Engine;

static std::string TOKEN_SCALE_TIME_KEYS = "ScaleTimeKeys";
static std::string TOKEN_ROTATE_TIME_KEYS = "RotateTimeKeys";
static std::string TOKEN_TRANSLATE_TIME_KEYS = "TranslateTimeKeys";
static std::string TOKEN_NAME = "Name";
static std::string TOKEN_MESH_NODE_NAMES = "MeshNodeNames";
static std::string TOKEN_TIME_SRTS = "TimeSRTs";

AnimationTimeSRTDto AnimationTimeSRTDto::FromGenericDto(const GenericDto& dto)
{
    AnimationTimeSRTDto srt;
    if (const auto v = dto.TryGetValue<std::vector<float>>(TOKEN_SCALE_TIME_KEYS)) srt.ScaleTimeKeys() = v.value();
    if (const auto v = dto.TryGetValue<std::vector<float>>(TOKEN_ROTATE_TIME_KEYS)) srt.RotateTimeKeys() = v.value();
    if (const auto v = dto.TryGetValue<std::vector<float>>(TOKEN_TRANSLATE_TIME_KEYS)) srt.TranslateTimeKeys() = v.value();
    return srt;
}

GenericDto AnimationTimeSRTDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddOrUpdate(TOKEN_SCALE_TIME_KEYS, m_scaleTimeKeys);
    dto.AddOrUpdate(TOKEN_ROTATE_TIME_KEYS, m_rotateTimeKeys);
    dto.AddOrUpdate(TOKEN_TRANSLATE_TIME_KEYS, m_translateTimeKeys);
    return dto;
}

ModelAnimationAssetDto ModelAnimationAssetDto::FromGenericDto(const Engine::GenericDto& dto)
{
    ModelAnimationAssetDto model;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) model.Name() = v.value();
    if (const auto v = dto.TryGetValue<std::vector<std::string>>(TOKEN_MESH_NODE_NAMES)) model.MeshNodeNames() = v.value();
    if (const auto v = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_TIME_SRTS)) model.TimeSRTs() = v.value();
    return model;
}

GenericDto ModelAnimationAssetDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(ModelAnimationAsset::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_MESH_NODE_NAMES, m_meshNodeNames);
    dto.AddOrUpdate(TOKEN_TIME_SRTS, m_timeSrtDtos);
    return dto;
}
