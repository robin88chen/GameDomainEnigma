#include "PawnLoader.h"
#include "LevelEditorCommands.h"
#include "LevelEditorErrors.h"
#include "LevelEditorEvents.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Gateways/DtoJsonGateway.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"

using namespace LevelEditor;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;

PawnLoader::PawnLoader()
{
    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onBuildSceneGraphFailed = std::make_shared<EventSubscriber>([=](auto e) { OnBuildSceneGraphFailed(e); });
    EventPublisher::subscribe(typeid(BuildFactorySceneGraphFailed), m_onBuildSceneGraphFailed);
}

PawnLoader::~PawnLoader()
{
    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildFactorySceneGraphFailed), m_onBuildSceneGraphFailed);
    m_onBuildSceneGraphFailed = nullptr;
}

error PawnLoader::StartLoadingPawn(const std::string& full_path)
{
    if (full_path.empty()) return ErrorCode::emptyFilePath;
    auto file = FileSystem::Instance()->OpenFile(full_path, Read | Binary);
    if (!file) return ErrorCode::fileIOError;
    const size_t file_size = file->Size();
    const auto read_buf = file->Read(0, file_size);
    FileSystem::Instance()->CloseFile(file);
    if (!read_buf) return ErrorCode::fileIOError;
    const auto dtos = DtoJsonGateway::Deserialize(convert_to_string(read_buf.value(), file_size));
    if (dtos.empty()) return ErrorCode::fileIOError;
    m_pawnFullPath = full_path;
    CommandBus::post(std::make_shared<BuildSceneGraph>(m_pawnFullPath, dtos));
    return ErrorCode::ok;
}

void PawnLoader::OnSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != m_pawnFullPath) return;
    if (ev->GetTopLevelSpatial().empty()) return;
    auto pawn = std::dynamic_pointer_cast<Pawn>(ev->GetTopLevelSpatial()[0]);
    if (!pawn) return;
    EventPublisher::post(std::make_shared<PawnLoaded>(pawn, m_pawnFullPath));
}

void PawnLoader::OnBuildSceneGraphFailed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildFactorySceneGraphFailed>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() != m_pawnFullPath) return;
    EventPublisher::post(std::make_shared<LoadPawnFailed>(m_pawnFullPath, ev->GetErrorCode()));
}
