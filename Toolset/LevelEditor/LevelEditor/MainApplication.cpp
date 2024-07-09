#include <windows.h>
#include "MainForm.h"
#include "Platforms/MemoryMacro.h"
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/dragdrop.hpp>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
    CoInitialize(nullptr);
    meInitMemoryCheck();
    //_CrtSetBreakAlloc(10545);
    LevelEditor::MainForm form;
    form.initSubPanels();
    form.caption("Enigma Level Editor");
    form.size(nana::size(1440, 800));
    form.move(nana::point(60, 60));
    form.show();

    form.bindAssetToSceneGraphDragdrop();

    nana::exec();

    return 0;
}