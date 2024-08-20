#include "ModelAnimationAssembler.h"
#include "Animators/AnimationAssetQueries.h"
#include "ModelAnimationAsset.h"
#include "AnimationTimeSRTAssembler.h"

using namespace Enigma::Renderables;

static std::string TOKEN_MESH_NODE_NAMES = "MeshNodeNames";
static std::string TOKEN_TIME_SRTS = "TimeSRTs";

ModelAnimationAssembler::ModelAnimationAssembler(const Animators::AnimationAssetId& id) : AnimationAssetAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(ModelAnimationAsset::TYPE_RTTI.getName());
}

void ModelAnimationAssembler::nodeSRT(const std::string& node_name, const AnimationTimeSRT& srt)
{
    nodeSRT(node_name, AnimationTimeSRTAssembler::make(srt));
}

void ModelAnimationAssembler::nodeSRT(const std::string& node_name, const std::shared_ptr<AnimationTimeSRTAssembler>& assembler)
{
    m_nodeSRTs.emplace_back(node_name, assembler);
}

void ModelAnimationAssembler::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_factoryDesc.ClaimAsResourceAsset(name, filename, path_id);
}

Enigma::Engine::GenericDto ModelAnimationAssembler::assemble() const
{
    std::vector<std::string> meshNodeNames;
    std::vector<Engine::GenericDto> timeSRTs;
    for (auto& nodeSRT : m_nodeSRTs)
    {
        meshNodeNames.emplace_back(std::get<std::string>(nodeSRT));
        timeSRTs.emplace_back(std::get<std::shared_ptr<AnimationTimeSRTAssembler>>(nodeSRT)->assembleWithSorted());
    }
    Engine::GenericDto dto;
    dto.addOrUpdate(TOKEN_MESH_NODE_NAMES, meshNodeNames);
    dto.addOrUpdate(TOKEN_TIME_SRTS, timeSRTs);
    return dto;
}

ModelAnimationDisassembler::ModelAnimationDisassembler() : AnimationAssetDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(ModelAnimationAsset::TYPE_RTTI.getName());
}

void ModelAnimationDisassembler::disassemble(const Engine::GenericDto& dto)
{
    std::vector<std::string> meshNodeNames;
    std::vector<Engine::GenericDto> timeSRTs;
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_NODE_NAMES)) meshNodeNames = v.value();
    if (const auto v = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_TIME_SRTS)) timeSRTs = v.value();
    assert(meshNodeNames.size() == timeSRTs.size());
    m_nodeSRTs.reserve(meshNodeNames.size());
    for (unsigned i = 0; i < meshNodeNames.size(); i++)
    {
        m_nodeSRTs.emplace_back(meshNodeNames[i], AnimationTimeSRTDisassembler::srt(timeSRTs[i]));
    }
}
