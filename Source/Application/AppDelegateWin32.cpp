#include "AppDelegateWin32.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#if TARGET_PLATFORM == PLATFORM_WIN32
#include "../GraphicAPIDx11/GraphicAPIDx11.h"
#include "Controllers/GraphicMain.h"
#include "FileSystem/FileSystem.h"
#include "Renderer/RendererCommands.h"
#include "Frameworks/CommandBus.h"
#include <memory>
#include <chrono>

#define WM_MOUSEWHEEL_LEGACY 0x020A

using namespace std::chrono_literals;

using namespace Enigma::Application;
AppDelegate* AppDelegate::m_instance = nullptr;

AppDelegate::AppDelegate(const std::string app_name)
    : m_appName(app_name), m_atom(0), m_hwnd(NULL)
{
    m_instance = this;
    m_hasLogFile = false;
    m_graphicMain = nullptr;
}

AppDelegate::~AppDelegate()
{
    Destroy();
    if (m_atom)
    {
        UnregisterClass(m_appName.c_str(), 0);
    }
    m_instance = nullptr;
}

void AppDelegate::RegisterAppClass()
{
    WNDCLASS wc;
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.lpszClassName = m_appName.c_str();
    wc.lpszMenuName = nullptr;
    m_atom = RegisterClass(&wc);
}

void AppDelegate::Create()
{
    m_hwnd = CreateWindow(m_appName.c_str(), m_appName.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, 0, NULL);
    ShowWindow(m_hwnd, SW_NORMAL);
    UpdateWindow(m_hwnd);
}

void AppDelegate::Destroy()
{
    //! 在 WM_CLOSE已經銷毀
    /*if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = 0;
    }*/
}

void AppDelegate::Initialize(Graphics::IGraphicAPI::APIVersion /*api_ver*/, Graphics::IGraphicAPI::AsyncType useAsyncDevice,
                             const std::string& log_filename)
{
    if (log_filename.length() > 0)
    {
        Platforms::Logger::InitLoggerFile(log_filename);
        m_hasLogFile = true;
    }

    FileSystem::FileSystem::Create();
    InitializeMountPaths();

    m_graphicMain = menew Controllers::GraphicMain();
    m_graphicMain->InstallFrameworks();

    menew Devices::GraphicAPIDx11(useAsyncDevice);

    // 這兩個函式從建構子搬來，因為，在建構子裡，子類別的virtual function table 還沒成立
    // 而Create 會 call 很多window message，這樣的 m_instance 並不會導到子類別的函式上
    RegisterAppClass();
    Create();

    CoInitializeEx(NULL, COINIT_MULTITHREADED);  // for WIC Texture Loader

    InstallEngine();
}

void AppDelegate::RegisterMediaMountPaths(const std::string& media_path)
{
    /*if (media_path.length() > 0)
    {
        m_mediaRootPath = media_path[media_path.length() - 1] == '/' ? media_path : (media_path + "/");
    }
    if (FileSystem::Instance())
    {
        FileSystem::Instance()->AddMountPath(IMountPathPtr{
            menew StdMountPath(m_mediaRootPath, EffectMaterialManager::GetEffectFilePathID()) });
        FileSystem::Instance()->AddMountPath(IMountPathPtr{
            menew StdMountPath(m_mediaRootPath, ShaderManager::GetShaderCodePathID()) });
        FileSystem::Instance()->AddMountPath(IMountPathPtr{
            menew StdMountPath(m_mediaRootPath, TextureManager::GetImageFilePathID()) });
        FileSystem::Instance()->AddMountPath(IMountPathPtr{
            menew StdMountPath(m_mediaRootPath, GeometryData::GetDataFilePathID()) });
        FileSystem::Instance()->AddMountPath(IMountPathPtr{
            menew StdMountPath(m_mediaRootPath, AnimationAsset::GetAnimationFilePathID()) });
        FileSystem::Instance()->AddMountPath(IMountPathPtr{
            menew StdMountPath(m_mediaRootPath, GlyphBitmapLoader::GetGlyphFilePathID()) });
    }*/
}

void AppDelegate::Finalize()
{
    ShutdownEngine();

    std::this_thread::sleep_for(std::chrono::seconds(1)); // 放一點時間給thread 執行 cleanup
    Graphics::IGraphicAPI::Instance()->TerminateGraphicThread(); // 先跳出thread
    delete Graphics::IGraphicAPI::Instance();

    m_graphicMain->ShutdownFrameworks();
    SAFE_DELETE(m_graphicMain);

    if (m_hasLogFile)
    {
        Platforms::Logger::CloseLoggerFile();
    }
    delete FileSystem::FileSystem::Instance();

    CoUninitialize();
}

void AppDelegate::Run()
{
    // 主訊息迴圈:
    MSG msg;
    BOOL bGotMsg;
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
    while (WM_QUIT != msg.message)
    {
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
        bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

        if (bGotMsg)
        {
            // Translate and dispatch the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            OnIdle();
            //FrameUpdate();
            //RenderFrame();
        }
    }  // end while
}

void AppDelegate::OnFrameSizeChanged(int w, int h)
{
    Frameworks::CommandBus::Post(std::make_shared<Renderer::ResizePrimaryRenderTarget>(
        MathLib::Dimension{(unsigned)w, (unsigned)h}));
}

LRESULT CALLBACK AppDelegate::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO:  在此加入任何繪圖程式碼...
        EndPaint(hWnd, &ps);
        break;
    case WM_MOUSEMOVE:
        m_instance->OnMouseMove(wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        m_instance->OnMouseLeftDown(wParam, lParam);
        break;
    case WM_LBUTTONUP:
        m_instance->OnMouseLeftUp(wParam, lParam);
        break;
    case WM_LBUTTONDBLCLK:
        m_instance->OnMouseLeftDblClick(wParam, lParam);
        break;
    case WM_RBUTTONDOWN:
        m_instance->OnMouseRightDown(wParam, lParam);
        break;
    case WM_RBUTTONUP:
        m_instance->OnMouseRightUp(wParam, lParam);
        break;
    case WM_RBUTTONDBLCLK:
        m_instance->OnMouseRightDblClick(wParam, lParam);
        break;
    case WM_MBUTTONDOWN:
        m_instance->OnMouseMiddleDown(wParam, lParam);
        break;
    case WM_MBUTTONUP:
        m_instance->OnMouseMiddleUp(wParam, lParam);
        break;
    case WM_MBUTTONDBLCLK:
        m_instance->OnMouseMiddleDblClick(wParam, lParam);
        break;
    case WM_MOUSEHWHEEL:
    case WM_MOUSEWHEEL_LEGACY:
        m_instance->OnMouseWheel(wParam, lParam);
        break;
    case WM_CHAR:
        m_instance->OnKeyChar(wParam, lParam);
        break;
    case WM_KEYDOWN:
        m_instance->OnKeyDown(wParam, lParam);
        break;
    case WM_KEYUP:
        m_instance->OnKeyUp(wParam, lParam);
        break;
    case WM_SIZE:
        if ((wParam != SIZE_MAXHIDE) && (wParam != SIZE_MINIMIZED))
        {
            m_instance->OnFrameSizeChanged(LOWORD(lParam), HIWORD(lParam));
        }
        break;
    case WM_CLOSE:
        m_instance->OnClose(wParam, lParam);
        //PostQuitMessage(0);
        break;
    case WM_DESTROY:
        m_instance->OnDestroy(wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void AppDelegate::OnIdle()
{
    FrameUpdate();

    PrepareRender();
    RenderFrame();

    //todo : GPU 太慢的時候要睡久一點, 看要怎麼動態調整
    std::this_thread::sleep_for(5us);// std::chrono::microseconds(1)); // 要加sleep，否則render thread分不到時間, 換成 micro second 快一些些
}

void AppDelegate::FrameUpdate()
{
    if (m_graphicMain) m_graphicMain->FrameUpdate();
}

void AppDelegate::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseMove((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseLeftDown(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        SetFocus(m_hwnd);
        input->ProcessWinMouseLeftButtonDown(false, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseLeftUp(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseLeftButtonUp((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseLeftDblClick(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseLeftButtonDown(true, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseRightDown(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseRightButtonDown(false, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseRightUp(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseRightButtonUp((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseRightDblClick(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseRightButtonDown(true, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseMiddleDown(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseMiddleButtonDown(false, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseMiddleUp(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseMiddleButtonUp((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseMiddleDblClick(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseMiddleButtonDown(true, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinMouseWheel((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam),
            MouseKeyFlags{ (unsigned int)wParam });
    }*/
}
void AppDelegate::OnKeyChar(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinKeyChar((char)(wParam), (int)(short)LOWORD(lParam));
    }*/
}
void AppDelegate::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinKeyPressDown((unsigned int)(wParam), (unsigned int)HIWORD(lParam));
    }*/
}
void AppDelegate::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    /*if (InputHandlerService* input = ServiceManager::GetSystemServiceAs<InputHandlerService>())
    {
        input->ProcessWinKeyPressUp((unsigned int)(wParam), (unsigned int)HIWORD(lParam));
    }*/
}
void AppDelegate::OnClose(WPARAM , LPARAM )
{
    DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
}
void AppDelegate::OnDestroy(WPARAM , LPARAM )
{
    PostQuitMessage(0);
}
#endif
