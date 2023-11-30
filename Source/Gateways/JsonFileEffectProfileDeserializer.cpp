#include "JsonFileEffectProfileDeserializer.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "EffectProfileJsonGateway.h"
#include "FileSystem/FileSystemErrors.h"
#include "GameEngine/EngineErrors.h"
#include "GameEngine/EffectCompiler.h"

using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;

JsonFileEffectProfileDeserializer::JsonFileEffectProfileDeserializer()
{

}

void JsonFileEffectProfileDeserializer::InvokeDeserialize(const Ruid& ruid_deserializing, const std::string& param)
{
    m_gateway.cleanup();

    IFilePtr readFile = FileSystem::FileSystem::instance()->openFile(Filename(param), FileSystem::read | FileSystem::binary);
    if (!readFile)
    {
        Frameworks::EventPublisher::post(std::make_shared<EffectCompiler::DeserializeCompilingProfileFailed>(ruid_deserializing, FileSystem::ErrorCode::fileOpenError));
        return;
    }
    size_t filesize = readFile->size();
    auto read_buff = readFile->read(0, filesize);
    if (!read_buff)
    {
        Frameworks::EventPublisher::post(std::make_shared<EffectCompiler::DeserializeCompilingProfileFailed>(ruid_deserializing, FileSystem::ErrorCode::readFail));
        return;
    }
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    auto profile = m_gateway.Deserialize(read_json);
    if (profile)
    {
        Frameworks::EventPublisher::post(std::make_shared<EffectCompiler::CompilingProfileDeserialized>(ruid_deserializing, profile.value()));
    }
    else
    {
        Frameworks::EventPublisher::post(std::make_shared<EffectCompiler::DeserializeCompilingProfileFailed>(ruid_deserializing, Engine::ErrorCode::deserializeFail));
    }
}
