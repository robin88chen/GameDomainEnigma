#include "JsonFileDtoDeserializer.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "DtoJsonGateway.h"
#include "FileSystem/FileSystemErrors.h"
#include "GameEngine/DtoEvents.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/EngineErrors.h"

using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;
using namespace Enigma::Engine;

JsonFileDtoDeserializer::JsonFileDtoDeserializer() : IDtoDeserializer()
{

}

void JsonFileDtoDeserializer::InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param)
{
    IFilePtr readFile = FileSystem::FileSystem::Instance()->OpenFile(Filename(param), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    if (read_buff)
    {
        std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
        Frameworks::EventPublisher::Post(std::make_shared<GenericDtoDeserialized>(ruid_deserializing, DtoJsonGateway::Deserialize(read_json)));
    }
    else
    {
        Frameworks::EventPublisher::Post(std::make_shared<DeserializeDtoFailed>(ruid_deserializing, Engine::ErrorCode::fileIOError));
    }
}
