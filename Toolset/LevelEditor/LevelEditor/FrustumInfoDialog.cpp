#include "FrustumInfoDialog.h"
#include "SceneGraph/CameraFrustumCommands.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include "SceneGraph/Frustum.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "LevelEditorCommands.h"
#include "MathLib/MathGlobal.h"

using namespace LevelEditor;

FrustumInfoDialog::FrustumInfoDialog(nana::window owner, const std::string& frustum_name) : form(owner, nana::API::make_center(owner, 280, 200), nana::appear::decorate<>{})
{
    caption("Frustum Info");
    get_place().div("vert<fov_prompt margin=[10,20]><near_plane_prompt margin=[10,20]><far_plane_prompt margin=[10,20]><buttons margin=[10,40] gap=10>");
    m_fovPrompt = menew nana::label(*this, "Fov(Degree) : ");
    (*m_fovPrompt).text_align(nana::align::right);
    m_fovInputBox = new nana::textbox(*this, "0.0");
    m_nearPlanePrompt = menew nana::label(*this, "Near Plane : ");
    (*m_nearPlanePrompt).text_align(nana::align::right);
    m_nearPlaneInputBox = new nana::textbox(*this, "0.0");
    m_farPlanePrompt = menew nana::label(*this, "Far Plane : ");
    (*m_farPlanePrompt).text_align(nana::align::right);
    m_farPlaneInputBox = new nana::textbox(*this, "0.0");
    get_place()["fov_prompt"] << *m_fovPrompt << *m_fovInputBox;
    get_place()["near_plane_prompt"] << *m_nearPlanePrompt << *m_nearPlaneInputBox;
    get_place()["far_plane_prompt"] << *m_farPlanePrompt << *m_farPlaneInputBox;
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->OnOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->OnCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();

    RegisterHandlers();

    Enigma::Frameworks::CommandBus::Post(std::make_shared<Enigma::SceneGraph::QueryFrustum>(frustum_name));
}

FrustumInfoDialog::~FrustumInfoDialog()
{
    UnregisterHandlers();

    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
    SAFE_DELETE(m_fovPrompt);
    SAFE_DELETE(m_fovInputBox);
    SAFE_DELETE(m_nearPlanePrompt);
    SAFE_DELETE(m_nearPlaneInputBox);
    SAFE_DELETE(m_farPlanePrompt);
    SAFE_DELETE(m_farPlaneInputBox);
}

void FrustumInfoDialog::RegisterHandlers()
{
    m_onReplyFrustumQuery = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](auto e) { OnReplyFrustumQuery(e); });
    Enigma::Frameworks::EventPublisher::Subscribe(typeid(Enigma::SceneGraph::ReplyFrustumQuery), m_onReplyFrustumQuery);
    m_onQueryFrustumFailed = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](auto e) { OnReplyFrustumQuery(e); });
    Enigma::Frameworks::EventPublisher::Subscribe(typeid(Enigma::SceneGraph::QueryFrustumFailed), m_onQueryFrustumFailed);
}

void FrustumInfoDialog::UnregisterHandlers()
{
    Enigma::Frameworks::EventPublisher::Unsubscribe(typeid(Enigma::SceneGraph::ReplyFrustumQuery), m_onReplyFrustumQuery);
    m_onReplyFrustumQuery = nullptr;
    Enigma::Frameworks::EventPublisher::Unsubscribe(typeid(Enigma::SceneGraph::QueryFrustumFailed), m_onQueryFrustumFailed);
    m_onQueryFrustumFailed = nullptr;
}

void FrustumInfoDialog::OnOkButton(const nana::arg_click& arg)
{

}

void FrustumInfoDialog::OnCancelButton(const nana::arg_click& arg)
{

}

void FrustumInfoDialog::OnReplyFrustumQuery(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    if (auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::ReplyFrustumQuery>(e))
    {
        if (ev->GetFrustum())
        {
            m_fovInputBox->caption(std::to_string(ev->GetFrustum()->GetFov() * 180.0f / Enigma::MathLib::Math::PI));
            m_nearPlaneInputBox->caption(std::to_string(ev->GetFrustum()->GetNearPlaneZ()));
            m_farPlaneInputBox->caption(std::to_string(ev->GetFrustum()->GetFarPlaneZ()));
        }
    }
    else if (auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::QueryFrustumFailed>(e))
    {
        Enigma::Frameworks::CommandBus::Post(std::make_shared<OutputMessage>("QueryFrustumFailed : " + ev->GetError().message()));
    }
}
