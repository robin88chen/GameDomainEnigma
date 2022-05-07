#include <windows.h>
#include "MainForm.h"
#include <nana/gui.hpp>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
    AssetPackageTool::MainForm form;
    form.Initialize();
    form.caption("Asset Package Tool");
    form.size(nana::size(1024, 768));
    form.move(nana::point(60, 60));
    form.show();

    nana::exec();

    return 0;
}