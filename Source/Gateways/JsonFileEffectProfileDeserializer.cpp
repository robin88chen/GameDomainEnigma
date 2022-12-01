#include "JsonFileEffectProfileDeserializer.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GameEngine/EffectEvents.h"
#include "EffectProfileJsonGateway.h"
#include "FileSystem/FileSystemErrors.h"
#include "GameEngine/EngineErrors.h"

using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;

JsonFileEffectProfileDeserializer::JsonFileEffectProfileDeserializer()
{

}

JsonFileEffectProfileDeserializer::~JsonFileEffectProfileDeserializer()
{

}

void JsonFileEffectProfileDeserializer::InvokeDeserialize(const Ruid& ruid_deserializing, const std::string& param)
{
    m_gateway.Cleanup();

    IFilePtr readFile = FileSystem::FileSystem::Instance()->OpenFile(Filename(param), "rb");
    if (!readFile)
    {
        Frameworks::EventPublisher::Post(std::make_shared<DeserializeCompilingProfileFailed>(ruid_deserializing, FileSystem::ErrorCode::fileOpenError));
        return;
    }
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    if (!read_buff)
    {
        Frameworks::EventPublisher::Post(std::make_shared<DeserializeCompilingProfileFailed>(ruid_deserializing, FileSystem::ErrorCode::readFail));
        return;
    }
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    auto profile = m_gateway.Deserialize(read_json);
    if (profile)
    {
        Frameworks::EventPublisher::Post(std::make_shared<CompilingProfileDeserialized>(ruid_deserializing, profile.value()));
    }
    else
    {
        Frameworks::EventPublisher::Post(std::make_shared<DeserializeCompilingProfileFailed>(ruid_deserializing, Engine::ErrorCode::deserializeFail));
    }
}
