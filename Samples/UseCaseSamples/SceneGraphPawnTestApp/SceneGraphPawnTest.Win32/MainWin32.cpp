#include "SceneGraphPawnTest.h"
#include "Platforms/MemoryMacro.h"
#include <windows.h>
#include <tchar.h>
#include <assert.h>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    //meInitMemoryCheck();
    //_CrtSetBreakAlloc(157);
    SceneGraphPawnTest* app = menew SceneGraphPawnTest("scene graph pawn test");
    app->initialize(Enigma::Graphics::IGraphicAPI::APIVersion::API_Dx11, Enigma::Graphics::IGraphicAPI::AsyncType::UseAsyncDevice);
    RECT rc;
    GetClientRect(app->getWnd(), &rc);
    app->onFrameSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
    //app->CreateScene();
    app->run();
    //app->DestroyScene();
    app->finalize();

    delete app;
    return 0;
}
