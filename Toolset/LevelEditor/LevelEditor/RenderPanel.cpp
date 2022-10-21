#include "RenderPanel.h"
#include "Renderer/RendererCommands.h"
#include "Frameworks/CommandBus.h"
#include "MathLib/AlgebraBasicTypes.h"

using namespace LevelEditor;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;

RenderPanel::RenderPanel(nana::window wd) : nana::nested_form{ wd, nana::appear::bald<>{} }
{
    events().mouse_down([this](const nana::arg_mouse& arg) { this->OnMouseDown(arg); });
    events().mouse_up([this](const nana::arg_mouse& arg) { this->OnMouseUp(arg); });
    events().mouse_move([this](const nana::arg_mouse& arg) { this->OnMouseMove(arg); });
    events().mouse_wheel([this](const nana::arg_wheel& arg) { this->OnMouseWheel(arg); });
    events().resized([this](const nana::arg_resized& arg) { this->OnResized(arg); });
}

RenderPanel::~RenderPanel()
{
}

void RenderPanel::OnMouseDown(const nana::arg_mouse& arg)
{
}

void RenderPanel::OnMouseMove(const nana::arg_mouse& arg)
{
}

void RenderPanel::OnMouseUp(const nana::arg_mouse& arg)
{
}

void RenderPanel::OnMouseWheel(const nana::arg_wheel& arg)
{
}

void RenderPanel::OnResized(const nana::arg_resized& arg)
{
    CommandBus::Post(std::make_shared<ResizePrimaryRenderTarget>(Dimension{ arg.width, arg.height }));
}

std::tuple<float, float> RenderPanel::ClientXYToClippingXY(int x, int y)
{
    float clipping_x, clipping_y;
    clipping_x = (2.0f * (float)x) / (float)(size().width) - 1.0f;
    clipping_y = (-2.0f * (float)y) / (float)(size().height) + 1.0f;
    return { clipping_x, clipping_y };
}
