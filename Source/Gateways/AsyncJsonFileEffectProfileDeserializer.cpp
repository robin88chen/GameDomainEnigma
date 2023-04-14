#include "AsyncJsonFileEffectProfileDeserializer.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "EffectProfileJsonGateway.h"
#include "FileSystem/FileSystemErrors.h"
#include "GameEngine/EngineErrors.h"
#include "Platforms/PlatformLayer.h"
#include "GameEngine/EffectCompiler.h"

using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;

AsyncJsonFileEffectProfileDeserializer::AsyncJsonFileEffectProfileDeserializer()
{

}

AsyncJsonFileEffectProfileDeserializer::~AsyncJsonFileEffectProfileDeserializer()
{

}

void AsyncJsonFileEffectProfileDeserializer::InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param)
{
    Platforms::Debug::Printf("async invoke deserialize %s", param.c_str());
    m_ruid = ruid_deserializing;
    m_parameter = param;
    m_deserializing = std::async(std::launch::async, [self = weak_from_this()]() // 抓給自己的 shared_ptr, 要抓 weak, 否則相當於自己抓自己shared_ptr, 永遠無法釋放
        { if (!self.expired()) std::dynamic_pointer_cast<AsyncJsonFileEffectProfileDeserializer, IEffectCompilingProfileDeserializer>(self.lock())->DeserializeProcedure(); });
}

void AsyncJsonFileEffectProfileDeserializer::DeserializeProcedure()
{
    FutureFile readingFile = FileSystem::FileSystem::Instance()->AsyncOpenFile(Filename(m_parameter), "rb");
    while (!readingFile.valid() || (readingFile.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)) {}
    IFilePtr readFile = readingFile.get();
    if (!readFile)
    {
        Frameworks::EventPublisher::Post(std::make_shared<EffectCompiler::DeserializeCompilingProfileFailed>(m_ruid, FileSystem::ErrorCode::fileOpenError));
        return;
    }
    size_t filesize = readFile->Size();
    IFile::FutureRead read = readFile->AsyncRead(0, filesize);
    while (!read.valid() || (read.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)) {}
    auto buff = read.get();
    if (!buff)
    {
        Frameworks::EventPublisher::Post(std::make_shared<EffectCompiler::DeserializeCompilingProfileFailed>(m_ruid, FileSystem::ErrorCode::readFail));
        return;
    }
    std::string read_json = convert_to_string(buff.value(), buff->size());
    auto profile = m_gateway.Deserialize(read_json);
    if (profile)
    {
        Frameworks::EventPublisher::Post(std::make_shared<EffectCompiler::CompilingProfileDeserialized>(m_ruid, profile.value()));
    }
    else
    {
        Frameworks::EventPublisher::Post(std::make_shared<EffectCompiler::DeserializeCompilingProfileFailed>(m_ruid, Engine::ErrorCode::deserializeFail));
    }
}
