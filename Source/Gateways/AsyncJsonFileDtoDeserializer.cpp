#include "AsyncJsonFileDtoDeserializer.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "DtoJsonGateway.h"
#include "GameEngine/DtoEvents.h"
#include "Frameworks/EventPublisher.h"
#include "FileSystem/FileSystemErrors.h"
#include "Platforms/PlatformLayerUtilities.h"

using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::FileSystem;

AsyncJsonFileDtoDeserializer::AsyncJsonFileDtoDeserializer() : IDtoDeserializer(), m_ruid()
{
    m_gateway = std::make_shared<DtoJsonGateway>();
}

void AsyncJsonFileDtoDeserializer::invokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param)
{
    Platforms::Debug::Printf("async invoke deserialize %s", param.c_str());
    m_ruid = ruid_deserializing;
    m_parameter = param;
    m_deserializing = std::async(std::launch::async, [self = weak_from_this()]() // 抓給自己的 shared_ptr, 要抓 weak, 否則相當於自己抓自己shared_ptr, 永遠無法釋放
        { if (!self.expired()) std::dynamic_pointer_cast<AsyncJsonFileDtoDeserializer, IDtoDeserializer>(self.lock())->DeserializeProcedure(); });
}

void AsyncJsonFileDtoDeserializer::DeserializeProcedure()
{
    assert(m_gateway);
    FutureFile readingFile = FileSystem::FileSystem::instance()->asyncOpenFile(Filename(m_parameter), FileSystem::read | FileSystem::binary);
    while (!readingFile.valid() || (readingFile.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)) {}
    IFilePtr readFile = readingFile.get();
    if (!readFile)
    {
        Frameworks::EventPublisher::post(std::make_shared<DeserializeDtoFailed>(m_ruid, FileSystem::ErrorCode::fileOpenError));
        return;
    }
    size_t filesize = readFile->size();
    IFile::FutureRead read = readFile->asyncRead(0, filesize);
    while (!read.valid() || (read.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)) {}
    auto buff = read.get();
    if (!buff)
    {
        Frameworks::EventPublisher::post(std::make_shared<DeserializeDtoFailed>(m_ruid, FileSystem::ErrorCode::readFail));
        return;
    }
    std::string read_json = convert_to_string(buff.value(), buff->size());
    Frameworks::EventPublisher::post(std::make_shared<GenericDtoDeserialized>(m_ruid, m_gateway->deserialize(read_json)));
}
