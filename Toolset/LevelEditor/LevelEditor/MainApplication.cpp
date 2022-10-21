#include <windows.h>
#include "MainForm.h"
#include "Platforms/MemoryMacro.h"
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
    meInitMemoryCheck();
    //_CrtSetBreakAlloc(2022);
    LevelEditor::MainForm form;
    form.InitSubPanels();
    form.caption("Enigma Level Editor");
    form.size(nana::size(1440, 800));
    form.move(nana::point(60, 60));
    form.show();

    nana::exec();

    return 0;
}