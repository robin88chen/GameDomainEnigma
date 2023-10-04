#include "PawnEditService.h"
#include "LevelEditorErrors.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti PawnEditService::TYPE_RTTI{ "LevelEditor.PawnEditService", &ISystemService::TYPE_RTTI };

PawnEditService::PawnEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
}

PawnEditService::~PawnEditService()
{

}

ServiceResult PawnEditService::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult PawnEditService::OnTerm()
{
    return ServiceResult::Complete;
}

error PawnEditService::PutCandidatePawn(const std::string& full_path, const Enigma::MathLib::Vector3& position)
{
    return ErrorCode::ok;
}

