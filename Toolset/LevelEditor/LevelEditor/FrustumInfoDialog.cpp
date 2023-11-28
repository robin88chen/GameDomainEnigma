#include "FrustumInfoDialog.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/Frustum.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/QueryDispatcher.h"
#include "LevelEditorCommands.h"
#include "MathLib/MathGlobal.h"

using namespace LevelEditor;

FrustumInfoDialog::FrustumInfoDialog(nana::window owner, const std::string& camera_name) : form(owner, nana::API::make_center(owner, 280, 200), nana::appear::decorate<>{})
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
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();

    registerHandlers();

    queryCamera(camera_name);
}

FrustumInfoDialog::~FrustumInfoDialog()
{
    unregisterHandlers();

    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
    SAFE_DELETE(m_fovPrompt);
    SAFE_DELETE(m_fovInputBox);
    SAFE_DELETE(m_nearPlanePrompt);
    SAFE_DELETE(m_nearPlaneInputBox);
    SAFE_DELETE(m_farPlanePrompt);
    SAFE_DELETE(m_farPlaneInputBox);
}

void FrustumInfoDialog::registerHandlers()
{
}

void FrustumInfoDialog::unregisterHandlers()
{
}

void FrustumInfoDialog::onOkButton(const nana::arg_click& arg)
{
    if (m_camera.expired())
    {
        close();
        return;
    }
    if (!m_fovInputBox->caption().empty())
    {
        float fov = std::stof(m_fovInputBox->caption()) * Enigma::MathLib::Math::PI / 180.0f;
        if ((fov > 0.0f) && (fov < Enigma::MathLib::Math::HALF_PI))
        {
            m_camera.lock()->changeFrustumFov(fov);
        }
        else
        {
            Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Invalid fov value."));
        }
    }
    float near_plane = 0.0f;
    float far_plane = 0.0f;
    if (!m_nearPlaneInputBox->caption().empty())
    {
        near_plane = std::stof(m_nearPlaneInputBox->caption());
    }
    if (near_plane <= 0.0f)
    {
        Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Invalid near plane value."));
        return;
    }
    if (!m_farPlaneInputBox->caption().empty())
    {
        far_plane = std::stof(m_farPlaneInputBox->caption());
    }
    if (far_plane <= 0.0f)
    {
        Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Invalid far plane value."));
        return;
    }
    if (near_plane < far_plane)
    {
        m_camera.lock()->changeFrustumNearPlane(near_plane);
        m_camera.lock()->changeFrustumFarPlane(far_plane);
    }
    else
    {
        Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Invalid near/far plane value."));
    }
    close();
}

void FrustumInfoDialog::onCancelButton(const nana::arg_click& arg)
{
    close();
}

void FrustumInfoDialog::queryCamera(const std::string& camera_name)
{
    auto query = std::make_shared<Enigma::SceneGraph::QueryCamera>(camera_name);
    Enigma::Frameworks::QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        m_camera = query->getResult();
        m_fovInputBox->caption(std::to_string(m_camera.lock()->cullingFrustum().fov() * 180.0f / Enigma::MathLib::Math::PI));
        m_nearPlaneInputBox->caption(std::to_string(m_camera.lock()->cullingFrustum().nearPlaneZ()));
        m_farPlaneInputBox->caption(std::to_string(m_camera.lock()->cullingFrustum().farPlaneZ()));
    }
    else
    {
        Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("QueryCameraFailed : " + camera_name));
    }
}
