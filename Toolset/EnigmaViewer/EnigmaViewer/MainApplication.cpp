#include <windows.h>
#include "MainForm.h"
#include "Platforms/MemoryAllocMacro.h"
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
    meInitMemoryCheck();
    //_CrtSetBreakAlloc(2350);
    EnigmaViewer::MainForm form;
    form.Initialize();
    form.caption("Enigma Viewer");
    form.size(nana::size(1024, 768));
    form.move(nana::point(60, 60));
    form.show();

    nana::exec();

    return 0;
}